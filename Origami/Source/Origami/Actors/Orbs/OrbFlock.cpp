#include "Origami.h"

#include "Actors/Characters/Player/OrigamiCharacter.h"
#include "Actors/Orbs/AI/OrbAiController.h"
#include "Runtime/AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "OrbFlock.h"


///////////////////////////////////////////////////////////////////////////
// FOrbFlockMember

FVector FOrbFlockMember::ComputeAlignment(TArray<FOrbFlockMember>& members, float maxSpeed, float neighborRadius, float maxForce)
{
	FVector v = FVector::ZeroVector;

	int count = 0;
	for (int i = 0; i < members.Num(); i++)
	{
		FOrbFlockMember& orb = members[i];
		float distance = FVector::Dist(this->Transform.GetLocation(), orb.Transform.GetLocation());

		if (distance > 0 && distance < neighborRadius)
		{
			v += members[i].Velocity;
			count++;
		}
	}

	if (count > 0)
	{
		v /= count;
		v = v.GetClampedToMaxSize(maxForce);
	}

	return v;
}

FVector FOrbFlockMember::ComputeSeparation(TArray<FOrbFlockMember>& members, float maxSpeed, float separationRadius, float maxForce)
{
	FVector v = FVector::ZeroVector;
	int count = 0;

	for (int i = 0; i < members.Num(); i++)
	{
		FOrbFlockMember& orb = members[i];

		float distance = FVector::Dist(this->Transform.GetLocation(), orb.Transform.GetLocation());

		if (distance > 0 && distance < separationRadius)
		{
			FVector difference = this->Transform.GetLocation() - members[i].Transform.GetLocation();
			difference.Normalize();
			difference /= distance;
			v += difference;
			count++;
		}
	}

	if (count > 0)
	{
		v /= count;
		return v;
	}


	return FVector::ZeroVector;
}

FVector FOrbFlockMember::ComputeCohesion(TArray<FOrbFlockMember>& members, float maxSpeed, float neighborRadius, float maxForce)
{
	FVector v = FVector::ZeroVector;
	int count = 0;
	for (int i = 0; i < members.Num(); i++)
	{
		FOrbFlockMember& orb = members[i];

		float distance = FVector::Dist(this->Transform.GetLocation(), orb.Transform.GetLocation());
		if (distance > 0 && distance < neighborRadius)
		{
			v += members[i].Transform.GetLocation();
			count++;
		}
	}

	if (count > 0)
	{
		
		v = ComputeSteerTo(v, maxSpeed, maxForce);
		v /= count;
		return v;
	}

	return FVector::ZeroVector;
}

FVector FOrbFlockMember::ComputeSteerTo(FVector target, float maxSpeed, float maxForce)
{
	FVector v = FVector::ZeroVector;
	FVector targetDirection = target - this->Transform.GetLocation();
	float distance = targetDirection.Size();
	if (distance > 0)
	{
		targetDirection.Normalize();

		if (distance < 200.0)
		{
			targetDirection *= maxSpeed * (distance / 200.0);
		}
		else
		{
			targetDirection *= maxSpeed;
		}

		v = targetDirection - this->Velocity;
		v = v.GetClampedToMaxSize(maxForce);
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
		

		UPointLightComponent* Light = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, TEXT("PointLight"));
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
	FVector cohesion = member.ComputeCohesion(this->Orbs, this->Simulation.FlockMaxSpeed, Simulation.NeighborRadius, Simulation.MaxForce) * this->Simulation.CohesionWeight;
	FVector alignment = member.ComputeAlignment(this->Orbs, this->Simulation.FlockMaxSpeed, Simulation.NeighborRadius, Simulation.MaxForce) * this->Simulation.AlignmentWeight;
	FVector separation = member.ComputeSeparation(this->Orbs, this->Simulation.FlockMaxSpeed, Simulation.SeparationRadius, Simulation.MaxForce) * this->Simulation.SeparationWeight;
	FVector steerTo = member.ComputeSteerTo(Orbs[0].Target, this->Simulation.FlockMaxSpeed, Simulation.MaxForce) * this->Simulation.SteerToTargetWeight;
	FVector acceleration = separation + alignment + cohesion + steerTo;

	member.Velocity += acceleration;
	member.Velocity = member.Velocity.GetClampedToMaxSize(this->Simulation.FlockMaxSpeed);

	FRotator rot = FindLookAtRotation(member.Transform.GetLocation(), member.Transform.GetLocation() + member.Velocity);
	//FRotator final = FMath::RInterpTo(orb.Transform.Rotator(), rot, deltaSeconds, 0.3f);

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
		this->SetActorLocation(member.Transform.GetLocation());

	if (Debug.bShowDebug)
		DrawDebugInformation(member, cohesion, separation, alignment);
}

void AOrbFlock::DrawDebugInformation(FOrbFlockMember& member, FVector cohesion, FVector separation, FVector alignment)
{
	cohesion.Normalize();
	alignment.Normalize();
	separation.Normalize();

	if (member.bIsLeader && Debug.bShowTarget)
		DrawDebugDirectionalArrow(GetWorld(), member.Transform.GetLocation(), member.Target, 100.0f, FColor::Magenta, false, -1.0f, '\000', 5.0f);

	if(Debug.bShowCohesion)
		DrawDebugDirectionalArrow(GetWorld(), member.Transform.GetLocation(), member.Transform.GetLocation() + cohesion * 50, 100.0f, FColor::Green, false, -1.0f, '\000', Debug.ArrowScale);

	if(Debug.bShowAlignment)
		DrawDebugDirectionalArrow(GetWorld(), member.Transform.GetLocation(), member.Transform.GetLocation() + alignment * 50, 100.0f, FColor::Red, false, -1.0f, '\000', Debug.ArrowScale);

	if(Debug.bShowSeparation)
		DrawDebugDirectionalArrow(GetWorld(), member.Transform.GetLocation(), member.Transform.GetLocation() + separation * 50, 100.0f, FColor::Blue, false, -1.0f, '\000', Debug.ArrowScale);

	if(Debug.bShowVelocity)
		DrawDebugDirectionalArrow(GetWorld(), member.Transform.GetLocation(), member.Transform.GetLocation() + member.Velocity, 100.0f, FColor::Black, false, -1.0f, '\000', Debug.ArrowScale);
}

FVector AOrbFlock::CalculateNewTarget()
{
	FOrbFlockMember& leader = this->Orbs[0];
	FVector targetVelocity = this->Orbs[0].Target - this->Orbs[0].Transform.GetLocation();

	leader.Target = this->GetRandomTarget() + this->GetActorLocation();

	return leader.Target;
}



