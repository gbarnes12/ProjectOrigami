#include "Origami.h"

#include "Actors/Characters/Player/OrigamiCharacter.h"
#include "Actors/Orbs/AI/OrbAiController.h"
#include "Runtime/AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "OrbGroup.h"
#include "DrawDebugHelpers.h"
#include "OrbFlock.h"


///////////////////////////////////////////////////////////////////////////
// FOrbFlockMember

FVector FOrbFlockMember::ComputeAlignment(TArray<FOrbFlockMember>& members, float maxSpeed, float neighborRadius, float maxForce)
{
	FVector v = FVector::ZeroVector;

	for (int i = 0; i < members.Num(); i++)
	{
		FOrbFlockMember& orb = members[i];
		v += members[i].Velocity;
	}

	if (members.Num() > 0)
	{
		v /= members.Num();
		v = v.GetClampedToMaxSize(maxForce);
	}

	return v;
}

FVector FOrbFlockMember::ComputeSeparation(TArray<FOrbFlockMember>& members, float maxSpeed, float maxForce)
{
	FVector v = FVector::ZeroVector;

	for (int i = 0; i < members.Num(); i++)
	{
		FOrbFlockMember& orb = members[i];
		float distance = FVector::Dist(this->Transform.GetLocation(), orb.Transform.GetLocation());

		FVector difference = this->Transform.GetLocation() - members[i].Transform.GetLocation();
		difference.Normalize();
		difference /= distance;
		v += difference;
	}

	if (members.Num() > 0)
	{
		v /= members.Num();
		return v;
	}


	return FVector::ZeroVector;
}

FVector FOrbFlockMember::ComputeCohesion(TArray<FOrbFlockMember>& members, float maxSpeed, float neighborRadius, float maxForce)
{
	FVector v = FVector::ZeroVector;

	for (int i = 0; i < members.Num(); i++)
	{
		FOrbFlockMember& orb = members[i];
		v += members[i].Transform.GetLocation();
	}

	if (members.Num() > 0)
	{
		v = ComputeSteerTo(v, maxSpeed, maxForce);
		v /= members.Num();
		return v;
	}

	return FVector::ZeroVector;
}

FVector FOrbFlockMember::ComputeSteerTo(FVector target, float maxSpeed, float maxForce)
{
	FVector v = FVector::ZeroVector;
	FVector targetDirection = target - this->Transform.GetLocation();
	targetDirection.Normalize();
	targetDirection *= maxSpeed;

	v = targetDirection - this->Velocity;
	v = v.GetClampedToMaxSize(maxForce);
	return v;
}

FVector FOrbFlockMember::ComputeAvoidance(const AActor* actor, const UWorld* world,  FVector Target, float maxSpeed, float maxForce, bool bShowDebug)
{
	FVector v = FVector::ZeroVector;
	if (world == nullptr) 
	{
		return v;
	}
	
	FVector velocity = this->Velocity;

	velocity.Normalize();

	FVector start = this->Transform.GetLocation();
	FVector ahead = this->Transform.GetLocation() + velocity * 100.0f;

	FCollisionQueryParams rvTraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, actor);
	rvTraceParams.bTraceComplex = true;
	rvTraceParams.bTraceAsyncScene = true;
	rvTraceParams.bReturnPhysicalMaterial = false;

	FHitResult rvHit(ForceInit);

	/*world->SweepSingle(
		rvHit, //result
		start, //start
		ahead, //end
		ECollisionChannel::ECC_Visibility, //collision channel
		rvTraceParams);*/

	world->SweepSingleByChannel(rvHit, start, ahead, FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(10), rvTraceParams);

	

	if (rvHit.bBlockingHit)
	{
		FVector reflection = FMath::GetReflectionVector(this->Velocity, rvHit.ImpactNormal);
		reflection.Normalize();

		if (bShowDebug)
		{
			DrawDebugDirectionalArrow(world, rvHit.ImpactPoint, rvHit.ImpactPoint + reflection * 200.0f, 100.0f, FColor::Green, false, -1.0f, '\000', 2.0f);
			DrawDebugDirectionalArrow(world, start, ahead, 100.0f, FColor::Green, false, -1.0f, '\000', 2.0f);
			DrawDebugSphere(world, ahead, 10, 5, FColor::Green, false, -1.0f, '\000');
		}

		reflection = reflection.GetClampedToMaxSize(maxForce);

		return reflection;

	}
	else
	{
		if (bShowDebug)
		{
			DrawDebugDirectionalArrow(world, start, ahead, 100.0f, FColor::Red, false, -1.0f, '\000', 2.0f);
			DrawDebugSphere(world, ahead, 10, 5, FColor::Red, false, -1.0f, '\000');
		}
	}

	

	return v;
}


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
		StaticMeshInstanceComponent->bCastDynamicShadow = true;
		StaticMeshInstanceComponent->CastShadow = true;
		StaticMeshInstanceComponent->bCastStaticShadow = true;

		Light = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, TEXT("PointLight"));
		if (Light)
		{
			Light->SetRelativeRotation(FRotator::ZeroRotator);
			Light->SetLightColor(FLinearColor::White);
			Light->SetIntensity(10000);
			Light->SetAttenuationRadius(1000);
			Light->SetSourceRadius(0.0f);
			Light->SetSourceLength(0.0f);
			Light->SetCastShadows(true);
			Light->AttachTo(StaticMeshInstanceComponent);
		}

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

	if (this->AiController != nullptr && this->AiController->BlackboardComponent != nullptr)
	{
		this->AiController->BlackboardComponent->SetValueAsVector(FName("Target"), this->Orbs[0].Target);
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

		SimulateOrbMember(deltaSeconds, orb);

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

TArray<FOrbFlockMember> AOrbFlock::FindNeighbors(FOrbFlockMember& member)
{
	TArray<FOrbFlockMember> members;

	for (int i = 0; i < this->Orbs.Num(); i++)
	{
		FOrbFlockMember& orb = this->Orbs[i];

		float distance = FVector::Dist(member.Transform.GetLocation(), orb.Transform.GetLocation());
		if (distance > 0 && distance < Simulation.NeighborRadius)
		{
			members.Add(orb);
		}
	}

	return members;
}

void AOrbFlock::AddFlockMember(const FTransform& transform, bool bIsLeader)
{
	int instanceId = StaticMeshInstanceComponent->AddInstanceWorldSpace(transform);
	FOrbFlockMember data;

	const FString particleSystemName = "PS_" + FString::FromInt(instanceId);
	UParticleSystemComponent* psComp = NewObject<UParticleSystemComponent>(this, UParticleSystemComponent::StaticClass(), FName(*particleSystemName));
	if (psComp)
	{
		psComp->SetTemplate(this->ParticleSystem);
		psComp->SetRelativeRotation(FRotator::ZeroRotator);
		psComp->RegisterComponent();
		psComp->SetWorldScale3D(FVector(this->Visual.OrbScales, this->Visual.OrbScales, this->Visual.OrbScales));
		data.Trail = psComp;
	}

	data.bIsLeader = bIsLeader;
	data.MeshInstanceId = instanceId;
	data.Transform = transform;
	data.Target = GetRandomTarget() + this->GetActorLocation();
	data.Velocity = (bIsLeader) ? data.Target - transform.GetLocation() : this->Orbs[0].Target - transform.GetLocation(); 
	this->Orbs.Add(data);
}

void AOrbFlock::SimulateOrbMember(float deltaSeconds, FOrbFlockMember& member)
{
	// Check for collision event if so we need to calculate an avoidance target
	// and if we need to avoid set the new target temporary till we reached it!
	TArray<FOrbFlockMember> neighbors = this->FindNeighbors(member);

	FVector acceleration = FVector::ZeroVector;
	FVector cohesion = member.ComputeCohesion(neighbors, this->Simulation.FlockMaxSpeed, Simulation.NeighborRadius, Simulation.MaxForce); 
	FVector alignment = member.ComputeAlignment(neighbors, this->Simulation.FlockMaxSpeed, Simulation.NeighborRadius, Simulation.MaxForce); 
	FVector separation = member.ComputeSeparation(neighbors, this->Simulation.FlockMaxSpeed, Simulation.MaxForce); 
	FVector steerTo = member.ComputeSteerTo(Orbs[0].Target, this->Simulation.FlockMaxSpeed, Simulation.MaxForce);
	FVector avoidance = member.ComputeAvoidance(this, GetWorld(), Orbs[0].Target, Simulation.FlockMaxSpeed, Simulation.MaxForce, (Debug.bShowDebug && Debug.bShowCollision) ? true : false);

	cohesion *= this->Simulation.CohesionWeight;
	alignment *= this->Simulation.AlignmentWeight;
	separation *= this->Simulation.SeparationWeight;
	steerTo *= this->Simulation.SteerToTargetWeight;
	avoidance *= this->Simulation.AvoidanceWeight;
	

	if (avoidance.Equals(FVector::ZeroVector))
		acceleration = separation + alignment + cohesion + steerTo;
	else
		acceleration = avoidance + steerTo;

	member.Velocity += acceleration;
	member.Velocity = member.Velocity.GetClampedToMaxSize(this->Simulation.FlockMaxSpeed);

	
	// we calculate the rotation by finding the direction we look to by the calculated velocity!
	FRotator rot = FindLookAtRotation(member.Transform.GetLocation(), member.Transform.GetLocation() + member.Velocity);

	member.Transform.SetRotation(rot.Quaternion());
	member.Transform.SetLocation(member.Transform.GetLocation() + member.Velocity * deltaSeconds);

	// Move animation trail according to movement of orb!
	if(member.Trail) 
	{
		FVector forward = member.Transform.GetUnitAxis(EAxis::X);
		forward.Normalize();

		FVector up = member.Transform.GetUnitAxis(EAxis::Z);
		up.Normalize();

		member.Trail->SetWorldLocation(member.Transform.GetLocation() + (forward * -1.0 * 10.0f) + (up * -1.0f * 2.5f));
		member.Trail->SetWorldRotation(rot.Quaternion());
	}

	StaticMeshInstanceComponent->UpdateInstanceTransform(member.MeshInstanceId, member.Transform, true, true);
	
	if (member.bIsLeader)
		this->Light->SetWorldLocation(member.Transform.GetLocation());

	if (member.bIsLeader && Simulation.bMoveActor) 
		this->SetActorLocation(member.Transform.GetLocation());

	if (Debug.bShowDebug)
		DrawDebugInformation(member, cohesion, separation, alignment);
}

void AOrbFlock::DrawDebugInformation(FOrbFlockMember& member, FVector cohesion, FVector separation, FVector alignment)
{
	cohesion.Normalize();
	alignment.Normalize();
	separation.Normalize();

	if (member.bIsLeader)
	{
		if(Debug.bShowTarget)
			DrawDebugDirectionalArrow(GetWorld(), member.Transform.GetLocation(), member.Target, 100.0f, FColor::Magenta, false, -1.0f, '\000', 5.0f);

		DrawDebugSphere(GetWorld(), member.Transform.GetLocation(), Simulation.NeighborRadius, 10, FColor::Green, false, -1.0f, '\000');
	}


	if(Debug.bShowCohesion)
		DrawDebugDirectionalArrow(GetWorld(), member.Transform.GetLocation(), member.Transform.GetLocation() + cohesion * 50, 100.0f, FColor::Green, false, -1.0f, '\000', Debug.ArrowScale);

	if(Debug.bShowAlignment)
		DrawDebugDirectionalArrow(GetWorld(), member.Transform.GetLocation(), member.Transform.GetLocation() + alignment * 50, 100.0f, FColor::Red, false, -1.0f, '\000', Debug.ArrowScale);

	if(Debug.bShowSeparation)
		DrawDebugDirectionalArrow(GetWorld(), member.Transform.GetLocation(), member.Transform.GetLocation() + separation * 50, 100.0f, FColor::Blue, false, -1.0f, '\000', Debug.ArrowScale);

	if(Debug.bShowVelocity)
		DrawDebugDirectionalArrow(GetWorld(), member.Transform.GetLocation(), member.Transform.GetLocation() + member.Velocity, 100.0f, FColor::Black, false, -1.0f, '\000', Debug.ArrowScale);
}

///////////////////////////////////////////////////////////////////////////
// Blueprint callable

bool AOrbFlock::IsLeaderAtLocation(FVector location, float thresholdDistance)
{
	if (this->Orbs.Num() == 0)
		return false;

	FOrbFlockMember& leader = this->Orbs[0];

	if (!leader.bIsLeader)
		return false;

	FVector leaderLocation = leader.Transform.GetLocation();

	float distance = (leaderLocation - location).Size();
	if (distance <= thresholdDistance) {
		return true;
	}

	return false;
}

void AOrbFlock::AttachToEntity(AActor* entity)
{
	if (!IsValid(entity))
		return;

	if (this->AiController != nullptr && this->AiController->BlackboardComponent != nullptr)
	{
		this->AiController->BlackboardComponent->SetValueAsObject(FName("AttachedActor"), entity);
		this->AiController->BlackboardComponent->SetValueAsEnum(FName("Mode"), (uint8)EOrbMode::Attached);
	}
}

void AOrbFlock::DetachFromEntity()
{
	if (this->AiController != nullptr && this->AiController->BlackboardComponent != nullptr)
	{
		this->AiController->BlackboardComponent->ClearValue(FName("AttachedActor"));
		this->AiController->BlackboardComponent->SetValueAsEnum(FName("Mode"), (uint8)EOrbMode::FreeRoam);
	}
}

FVector AOrbFlock::CalculateNewTarget()
{
	FOrbFlockMember& leader = this->Orbs[0];

	FVector targetVelocity = this->Orbs[0].Target - this->Orbs[0].Transform.GetLocation();
	leader.Target = this->GetRandomTarget() + this->GetActorLocation();
	return leader.Target;
}



