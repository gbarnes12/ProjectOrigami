#include "Origami.h"

#include "Actors/Characters/Player/OrigamiCharacter.h"
#include "DrawDebugHelpers.h"
#include "OrbFlock.h"


///////////////////////////////////////////////////////////////////////////
// AOrbFlock

AOrbFlock::AOrbFlock(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->Tags.Add(TEXT("OrbFlock"));

	PrimaryActorTick.bCanEverTick = true;

	// Root Scene Component
	this->RootSceneComponent = CreateAbstractDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	if (this->RootSceneComponent)
	{
		this->RootSceneComponent->RelativeLocation = FVector::ZeroVector;
		this->RootComponent = this->RootSceneComponent;
	}

	this->SphereComponent = CreateAbstractDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	if (this->SphereComponent)
	{
		this->SphereComponent->AttachTo(RootComponent);
		this->SphereComponent->SetRelativeLocation(FVector::ZeroVector);
		this->SphereComponent->SetRelativeRotation(FRotator::ZeroRotator);
		this->SphereComponent->RegisterComponent();
	}

	this->StaticMeshInstanceComponent = ObjectInitializer.CreateDefaultSubobject<UInstancedStaticMeshComponent>(this, TEXT("FlockMeshes"));
	if (StaticMeshInstanceComponent)
	{
		this->SphereComponent->RegisterComponent();
		this->StaticMeshInstanceComponent->AttachParent = RootComponent;
		
	}
	
}

///////////////////////////////////////////////////////////////////////////
// UE4 Native Events

void AOrbFlock::BeginPlay()
{
	this->StaticMeshInstanceComponent->SetStaticMesh(Mesh);

	for (int i = 0; i < this->Visual.OrbCount; i++)
	{
		FVector pos = FMath::VRand();
		float randSeed = FMath::FRandRange(0.01, 100);

		pos *= randSeed;
		pos += this->GetActorLocation();

		AddFlockMember(FTransform(FQuat::Identity, pos, FVector(this->Visual.OrbScales, this->Visual.OrbScales, this->Visual.OrbScales)), (i == 0) ? true : false);
	}
}

void AOrbFlock::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	for (int i = 0; i < this->Visual.OrbCount; i++)
	{
		FOrbFlockMember&  orb = Orbs[i];
		FVector pos = orb.Transform.GetLocation();

		if (orb.MeshInstanceId > StaticMeshInstanceComponent->GetInstanceCount())
			continue;

		FVector velocity = FVector(0, 0, 0);

		orb.ElapsedTimeSinceTargetUpdate += deltaSeconds;

		FVector wander = ComputeWanderVelocity(orb);
		FVector alignment = ComputeAlignment(orb);
		FVector cohesion = ComputeCohesion(orb);
		FVector separation = ComputSeparation(orb) ;
		FVector follow = ComputeFollow(orb);

	

		if (!orb.bIsLeader)
		{
			if (Simulation.FollowWeight != 0.0)
				velocity += follow * Simulation.FollowWeight;
		}
			
		if (Simulation.CohesionWeight != 0.0)
			velocity += cohesion  * Simulation.CohesionWeight;

		if (Simulation.AlignmentWeight != 0.0)
			velocity += alignment  * Simulation.AlignmentWeight;

		if (Simulation.SeparationWeight != 0.0)
			velocity += separation * Simulation.SeparationWeight;

		velocity += wander;

		velocity = velocity.GetClampedToSize(0.0f, 100.0f);

		FVector targetVelocity = orb.Velocity + velocity;
		FRotator rot = FindLookAtRotation(orb.Transform.GetLocation(), orb.Transform.GetLocation() + targetVelocity);
		FRotator final = FMath::RInterpTo(orb.Transform.Rotator(), rot, deltaSeconds, 0.3f);
		orb.Transform.SetRotation(final.Quaternion());

		FVector forward = orb.Transform.GetUnitAxis(EAxis::X);
		forward.Normalize();

		orb.Velocity = forward * targetVelocity.Size();

		if (this->Simulation.bUseCollisionAvoidance)
		{
			FVector avoidance = ComputeAvoidance(orb, orb.Velocity, Orbs[0].Target);
			orb.Velocity += avoidance;
		}
		
		if (orb.Velocity.Size() > Simulation.FlockMaxSpeed)
			orb.Velocity = orb.Velocity.GetSafeNormal() * Simulation.FlockMaxSpeed;

		if (orb.Velocity.Size() < Simulation.FlockMinSpeed)
			orb.Velocity = orb.Velocity.GetSafeNormal() * Simulation.FlockMinSpeed;


		if (orb.bIsLeader) 
		{
			this->CalculateNewTarget();
		//	this->SetActorLocation(orb.Transform.GetLocation() + orb.Velocity * deltaSeconds);
			DrawDebugDirectionalArrow(GetWorld(), pos, orb.Target, 100.0f, FColor::Red, false, -1.0f, '\000', 5.0f);
		}

		orb.Transform.SetLocation(orb.Transform.GetLocation() + orb.Velocity * deltaSeconds);
		StaticMeshInstanceComponent->UpdateInstanceTransform(orb.MeshInstanceId, orb.Transform, true, true);
	}

	StaticMeshInstanceComponent->UpdateBounds();
}


///////////////////////////////////////////////////////////////////////////
// Gameplay

FRotator AOrbFlock::FindLookAtRotation(FVector start, FVector end)
{
	return FRotationMatrix::MakeFromX(end - start).Rotator();
}

FVector AOrbFlock::GetRandomTarget()
{
	return FMath::VRand() * (FMath::FRand() * this->SphereComponent->GetScaledSphereRadius());
}

void AOrbFlock::AddFlockMember(const FTransform& transform, bool bIsLeader)
{
	int instanceId = StaticMeshInstanceComponent->AddInstanceWorldSpace(transform);
	FOrbFlockMember data;
	data.bIsLeader = bIsLeader;
	data.MeshInstanceId = instanceId;
	data.Transform = transform;
	data.Target = GetRandomTarget() + this->GetActorLocation();
	data.Velocity = (bIsLeader) ? data.Target : this->Orbs[0].Target - transform.GetLocation(); // ComputeWanderVelocity(data);
	this->Orbs.Add(data);
}

FVector AOrbFlock::ComputeFollow(FOrbFlockMember& member)
{
	FVector velocity = FVector::ZeroVector;
	FOrbFlockMember& leader = this->Orbs[0];

	velocity = leader.Transform.GetLocation() - member.Transform.GetLocation();
	velocity.Normalize();
	velocity *= this->Simulation.FlockMaxSpeed;
	velocity -= member.Velocity;

	return velocity;
}

void AOrbFlock::CalculateNewTarget()
{
	FOrbFlockMember& leader = this->Orbs[0];
	FVector targetVelocity = this->Orbs[0].Target - this->Orbs[0].Transform.GetLocation();

	// do we have reached a target min threshold yet? 
	// if not calculate a new target and target velocity!
	if (leader.ElapsedTimeSinceTargetUpdate > Visual.FlockWanderUpdateRate || targetVelocity.Size() <= this->Visual.TargetMinThreshold)
	{
		leader.Target = this->GetRandomTarget() + this->GetActorLocation();
	}
}

FVector AOrbFlock::ComputeAvoidance(FOrbFlockMember& member, FVector velocity, FVector target)
{
	velocity.Normalize();

	//FVector forward = member.Transform.GetUnitAxis(EAxis::X);
	FVector start = member.Transform.GetLocation();
	FVector ahead = member.Transform.GetLocation() + velocity * 1000.0;

	FVector avoidanceForce = FVector(0.0f, 0.0f, 0.0f);

	FCollisionQueryParams rvTraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	rvTraceParams.bTraceComplex = true; 
	rvTraceParams.bTraceAsyncScene = true;
	rvTraceParams.bReturnPhysicalMaterial = false;

	FHitResult rvHit(ForceInit);

	GetWorld()->LineTraceSingleByChannel(
		rvHit, //result
		start, //start
		ahead, //end
		ECollisionChannel::ECC_Visibility, //collision channel
		rvTraceParams);

	if (rvHit.bBlockingHit)
	{
		if (rvHit.GetActor()) {
			avoidanceForce = ahead - rvHit.GetActor()->GetActorLocation();
			avoidanceForce.Normalize();
			avoidanceForce *= 1000;
			 
			DrawDebugDirectionalArrow(GetWorld(), rvHit.ImpactPoint, avoidanceForce, 100.0f, FColor::Green, false, -1.0f, '\000', 5.0f);
			DrawDebugDirectionalArrow(GetWorld(), start, ahead, 100.0f, FColor::Red, false, -1.0f, '\000', 5.0f);
		}
	}
	else
	{
		if (member.bIsLeader)
			DrawDebugDirectionalArrow(GetWorld(), start, ahead, 100.0f, FColor::Red, false, -1.0f, '\000', 5.0f);
	}

	return avoidanceForce;
}

FVector AOrbFlock::ComputeWanderVelocity(FOrbFlockMember& member)
{
	if (this->Orbs.Num() == 0)
		return FVector(0.0f, 0.0f, 0.0f);

	// Calculates the difference between the target of the flock member and the current location of it
	FVector targetVelocity = this->Orbs[0].Target - member.Transform.GetLocation();

	return targetVelocity;
}

FVector AOrbFlock::ComputeAlignment(FOrbFlockMember& member)
{
	FVector v = FVector::ZeroVector;

	for (int i = 0; i < this->Orbs.Num(); i++) 
	{
		FOrbFlockMember& orb = this->Orbs[i];
		v += this->Orbs[i].Velocity;
	}

	v /= (float)this->Orbs.Num();
	return v;
}

FVector AOrbFlock::ComputSeparation(FOrbFlockMember& member)
{
	FVector v = FVector::ZeroVector;

	for (int i = 0; i < this->Orbs.Num(); i++) 
	{
		FOrbFlockMember& orb = this->Orbs[i];
		FVector difference = member.Transform.GetLocation() - this->Orbs[i].Transform.GetLocation();
		float scale = difference.Size();
		difference.Normalize();

		v += difference;
	}
	return v;
}

FVector AOrbFlock::ComputeCohesion(FOrbFlockMember& member)
{
	FVector v = FVector::ZeroVector;

	for (int i = 0; i < this->Orbs.Num(); i++)
	{
		FOrbFlockMember& orb = this->Orbs[i];
		v += this->Orbs[i].Transform.GetLocation();
	}

	// negate the result in order to properly let 
	// the flock mates steer away from each others
	v /= (float)this->Orbs.Num();
	return v - member.Transform.GetLocation();
}