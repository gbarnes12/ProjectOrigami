#include "Origami.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Actors/Characters/Player/OrigamiCharacter.h"
#include "Actors/Entities/Cocoon.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"
#include "Actors/Entity.h"
#include "Actors/OrbPath.h"
#include "OrbGroup.h"

///////////////////////////////////////////////////////////////////////////
// AOrbGroup

AOrbGroup::AOrbGroup(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->Tags.Add(TEXT("OrbGroup"));
	
	this->TravelledDistanceOnPath = 0.0f;
	this->Mode = EOrbMode::FreeRoam;
	this->OrbCount = 20;
	this->OrbColor = FColor::White;
	this->OrbSpawnBoxExtents = 20.0f;
	this->CurrentDissolveState = 0.0f;
	this->TargetDissolveState = 0.0f;
	this->bIsTargetMoving = false;
	this->bIsGenerated = false;
	this->bIsPaused = false;
	this->OrbMeshFileName = TEXT("StaticMesh'/Game/Origami/Objects/Orb/Visual/Obj_Stc_Orb.Obj_Stc_Orb'");
	this->OrbPath = NULL;
	
	// The socket functions as 
	this->Socket = NULL;

	// Root Scene Component
	this->RootSceneComponent = CreateAbstractDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	this->RootSceneComponent->RelativeLocation = FVector::ZeroVector;
	this->RootComponent = this->RootSceneComponent;

	// Add trigger component thus we can check whether the orbs collide with an object or not!
	this->Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerCollider"));
	this->Trigger->bGenerateOverlapEvents = true;
	this->Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->Trigger->AttachTo(this->RootSceneComponent);

	FOnTimelineFloat progressFunction;
	progressFunction.BindUFunction(this, TEXT("MoveToTarget"));
	const ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("CurveFloat'/Game/Origami/Data/LinearMovementCurve.LinearMovementCurve'"));
	this->MovementTimeline.AddInterpFloat(Curve.Object, progressFunction, TEXT("TEST"));
	
}

///////////////////////////////////////////////////////////////////////////
// UE4 Native Events

void AOrbGroup::BeginPlay()
{
	Super::BeginPlay();

	this->MovementSpeed = OrbSpeedLevel[0];

	//OnActorBeginOverlap.AddDynamic(this, &AOrbGroup::OnTriggerEnter);
	//OnActorEndOverlap.AddDynamic(this, &AOrbGroup::OnTriggerExit);

	// GenerateOrbs only at runtime 
	// if we do this within the constructor the editor becomes slow as fuck somehow?!
	this->GenerateOrbs();

	/*Start the timer that will adjust the speed over time*/
	GetWorldTimerManager().ClearTimer(AdjustSpeedTimerHandle);
	GetWorldTimerManager().SetTimer(AdjustSpeedTimerHandle, this, &AOrbGroup::AdjustSpeed, 5.0f, true);
}


void AOrbGroup::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//OnActorBeginOverlap.RemoveDynamic(this, &AOrbGroup::OnTriggerEnter);
	//OnActorEndOverlap.RemoveDynamic(this, &AOrbGroup::OnTriggerExit);

	Super::EndPlay(EndPlayReason);
}

void AOrbGroup::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	//UKismetSystemLibrary::DrawDebugBox(GetWorld(), this->RootComponent->GetComponentLocation(), FVector(200), FLinearColor::Red, FRotator::ZeroRotator, 0.1f);
	//UKismetSystemLibrary::DrawDebugBox(GetWorld(), this->BoxSceneComponent->GetComponentLocation(), this->BoxSceneComponent->GetUnscaledBoxExtent(), FLinearColor::Blue, FRotator::ZeroRotator, 0.1f);

	switch(this->Mode)
	{
	case EOrbMode::Attached:
		this->SimulateAttachment(deltaSeconds);
		break;
	case EOrbMode::FreeRoam:
		this->SimulateRoaming(deltaSeconds);
		break;
	}
}

///////////////////////////////////////////////////////////////////////////
// Trigger Events

void AOrbGroup::OnTriggerEnter(AActor* Other)
{

}

void AOrbGroup::OnTriggerExit(AActor* Other)
{

}


///////////////////////////////////////////////////////////////////////////
// Construction

void AOrbGroup::GenerateOrbs()
{
	// take care that we do not recreate the 
	// orbs!
	if (this->bIsGenerated)
		return;

	const FVector meshScale = FVector(0.20f, 0.20f, 0.20f);

	Light = NewObject<UPointLightComponent>(this, UPointLightComponent::StaticClass(), TEXT("PointLight"));
	if (Light)
	{
		Light->SetRelativeRotation(FRotator::ZeroRotator);
		Light->SetLightColor(FLinearColor::White);
		Light->SetIntensity(10000);
		Light->SetAttenuationRadius(1000);
		Light->SetSourceRadius(0.0f);
		Light->SetSourceLength(0.0f);
		Light->SetCastShadows(true);
		Light->AttachTo(this->RootComponent);
		Light->RegisterComponent();
	}

	// load static mesh
	this->OrbMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *this->OrbMeshFileName));
	if (!this->OrbMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't load orb mesh with filename %s"), *this->OrbMeshFileName);
		return;
	}

	FString particleSystemName = TEXT("StaticMesh'/Game/Origami/Objects/Orb/Visual/Ps_Orb_AnimTrail.Ps_Orb_AnimTrail'");

	UParticleSystem* ps = Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), NULL, *particleSystemName));
	if (!ps)
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't load particle system with name %s"), *particleSystemName);
		return;
	}

	this->OrbMaterialInstance = UMaterialInstanceDynamic::Create(this->OrbMesh->GetMaterial(0), this);
	if (!this->OrbMaterialInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't create dynamic material instance from mesh with filename %s"), *this->OrbMeshFileName);
		return;
	}

	// create the area in which we want to spawn the orbs 
	const FBox spawnBox = FBox::BuildAABB(this->GetActorLocation(), FVector(this->OrbSpawnBoxExtents));

	// we know how many orbs we need to create upfront 
	// so we can reserve the memory on the initial creation!
	for (uint16 i = 0; i < this->OrbCount; i++)
	{
		const FString staticMeshCompName = "Orb_" + FString::FromInt(i) + "_Mesh";
		const FName fMeshName = FName(*staticMeshCompName);
		const FVector location = FMath::RandPointInBox(spawnBox);

		// now we can create a static mesh component!
		UStaticMeshComponent* meshComp = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), fMeshName);
		if (meshComp)
		{
			meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			meshComp->SetStaticMesh(this->OrbMesh);
			meshComp->SetRelativeScale3D(meshScale);
			meshComp->SetRelativeRotation(FRotator::ZeroRotator);
			meshComp->SetWorldLocation(location);
			meshComp->AttachTo(this->RootComponent);
			meshComp->SetMaterial(0, this->OrbMaterialInstance);

			const FString particleSystemName = "PS_" + FString::FromInt(i);

			UParticleSystemComponent* psComp = NewObject<UParticleSystemComponent>(this, UParticleSystemComponent::StaticClass(), FName(*particleSystemName));
			if (psComp)
			{
				psComp->SetTemplate(ps);
				psComp->SetRelativeRotation(FRotator::ZeroRotator);
				psComp->AttachTo(meshComp, TEXT("AnimTrailSocket"));
				psComp->RegisterComponent();
			}
			meshComp->RegisterComponent();
		}
	}

	this->bIsGenerated = true;
}

///////////////////////////////////////////////////////////////////////////
// Timeline Methods

void AOrbGroup::MoveToTarget(float value)
{
	FVector currentPosition = FMath::Lerp(TargetInfo.StartLocation, TargetInfo.TargetLocation, value);
	FRotator currentRotation = UKismetMathLibrary::FindLookAtRotation(currentPosition, TargetInfo.TargetLocation);
	this->SetActorLocationAndRotation(currentPosition, currentRotation);
	
	if (value >= 1.0f)
	{
		// in case we want to attach this object to the target at the end 
		// do it!
		if (TargetInfo.bAttachToTargetAtEnd)
			this->AttachSocket(TargetInfo.Target);

		// if we have no target we need to destroy this actor!
		if (TargetInfo.Target == NULL)
			this->Destroy();

		// clear the target information
		TargetInfo.bAttachToTargetAtEnd = false;
		TargetInfo.StartLocation = FVector::ZeroVector;
		TargetInfo.TargetLocation = FVector::ZeroVector;
		TargetInfo.Target = NULL;

		this->MovementTimeline.Stop();
		
	}
}

void AOrbGroup::StartMoveToTarget(AActor* target, bool bAttachToTargetAtEnd)
{
	if (!IsValid(target))
		return;

	this->StartMoveToTarget(target, target->GetActorLocation(), bAttachToTargetAtEnd);
}

void AOrbGroup::StartMoveToTarget(AActor* target, FVector location, bool bAttachToTargetAtEnd)
{
	if (!IsValid(target))
	{
		bAttachToTargetAtEnd = false;
	}

	this->OrbSpeedLevel[0] = 100;
	this->OrbSpeedLevel[1] = 120;
	this->OrbSpeedLevel[2] = 140;

	// we detach the orb group from the socket first.
	DetachFromSocket();

	TargetInfo.Target = target;
	TargetInfo.TargetLocation = location;
	TargetInfo.StartLocation = this->GetActorLocation();
	TargetInfo.bAttachToTargetAtEnd = bAttachToTargetAtEnd;
	MovementTimeline.PlayFromStart();
}

////////////////////////////////////////////////////////////////////////////
// Visual Methods

void AOrbGroup::ChangeColor(FColor color)
{
	//UE_LOG(LogTemp, Info, TEXt("Teeeest"));
	this->OrbMaterialInstance->SetVectorParameterValue(TEXT("Color"), color);
	this->Light->SetLightColor(color);
}

///////////////////////////////////////////////////////////////////////////
// Socket Methods

void AOrbGroup::AttachSocket(AActor* socket)
{
	if (!IsValid(socket))
		return;
	
	this->AttachedType = EActorType::None;

	if (socket->ActorHasTag(TEXT("Player"))) 
	{
		AOrigamiCharacter* player = Cast<AOrigamiCharacter>(socket);
		if (!player)
			return;
		
		player->AddOrbGroup(this);

		this->AttachedType = EActorType::Player;
		this->Socket = socket;
		
		this->OrbPath = player->OrbPath;

		this->OrbSpeedLevel[0] = 100;
		this->OrbSpeedLevel[1] = 120;
		this->OrbSpeedLevel[2] = 140;
	} 
	else if (socket->ActorHasTag(TEXT("Entity")))
	{
		this->Socket = socket;
		this->AttachedType = EActorType::Entity;
		
		AEntity* entity = Cast<AEntity>(this->Socket);
		if (!IsValid(entity)) {
			DetachFromSocket();
			return;
		}

		// make the entity visible if necessary!
		entity->Dismantle();

		// since we have different types of entities 
		// we need to decide how we simulate the orbs!
		if (entity->GetClass()->IsChildOf(ACocoon::StaticClass()))
		{
			const ACocoon* cocoon = Cast<ACocoon>(this->Socket);
			this->OrbPath = cocoon->GetOrbPath();
		}
	} 
	else if (socket->ActorHasTag(TEXT("OrbPath")))
	{
		this->Socket = socket;
		this->AttachedType = EActorType::Path;

		const AOrbPath* path = Cast<AOrbPath>(this->Socket);
		if (!IsValid(path))
		{
			DetachFromSocket();
			return;
		}
		this->OrbPath = path->OrbPath;

		this->OrbSpeedLevel[0] = 1000;
		this->OrbSpeedLevel[1] = 1030;
		this->OrbSpeedLevel[2] = 1100;

	}

	this->K2_AttachRootComponentToActor(this->Socket, NAME_None, EAttachLocation::Type::KeepWorldPosition);
	this->TravelledDistanceOnPath = this->OrbPath->GetSplineLength();
	this->CurrentRotation = 0.0f;
	this->MovementSpeed = this->OrbSpeedLevel[0];
	this->Mode = EOrbMode::Attached;
}

void AOrbGroup::DetachFromSocket() 
{
	if (this->Socket == NULL)
		return;

	if (this->Socket->ActorHasTag(TEXT("OrbPath")))
	{
		AOrbPath* path = Cast<AOrbPath>(this->Socket);
		if (IsValid(path))
		{
			path->DetachFromGroup();
		}
	}
	
	this->DetachRootComponentFromParent(true);
	this->Socket = NULL;
	this->AttachedType = EActorType::None;
	this->Mode = EOrbMode::FreeRoam;
}


///////////////////////////////////////////////////////////////////////////
// Simulation Methods

void AOrbGroup::SimulateRoaming(float deltaSeconds)
{
	// Tick the timeline in case we use it :)
	if (this->MovementTimeline.IsPlaying()) 
		this->MovementTimeline.TickTimeline(deltaSeconds);
}

void AOrbGroup::SimulateAttachment(float deltaSeconds)
{
	if (!this->Socket)
		return;

	if (this->AttachedType == EActorType::None)
		return;

	// target is moving so we need to stop rotating around the path!
	if (this->bIsTargetMoving)
	{
		this->SetActorRotation(this->Socket->GetActorRotation());
		return;
	}

	if (this->bIsPaused) 
		return;

	this->FollowPath(deltaSeconds);
}

void AOrbGroup::FollowPath(float deltaSeconds) 
{
	if (!this->OrbPath )
		return;

	float pathDistance = 0.0f;
	this->TravelledDistanceOnPath = FMath::FInterpConstantTo(this->TravelledDistanceOnPath, pathDistance, deltaSeconds, this->MovementSpeed);

	if (this->TravelledDistanceOnPath <= pathDistance) 
	{
		if (this->AttachedType == EActorType::Path) 
		{
			AOrbPath* path = Cast<AOrbPath>(this->Socket);
			path->OrbsHaveReachedEnd.Broadcast(this);
			this->DetachFromSocket();
			return;
		}
		else 
		{
			this->TravelledDistanceOnPath = this->OrbPath->GetSplineLength();
		}
	}

	
	const FVector location = this->OrbPath->GetWorldLocationAtDistanceAlongSpline(TravelledDistanceOnPath);
	const FRotator rotation = this->OrbPath->GetWorldRotationAtDistanceAlongSpline(TravelledDistanceOnPath);
	
	this->SetActorLocationAndRotation(location, rotation, true);

	// in case we are attached to a path we need to 
	// do some specific stuff example handle waiting at certain
	// points and so on!
	if (this->AttachedType == EActorType::Path)
	{
		AOrbPath* path = Cast<AOrbPath>(this->Socket);
		if (!IsValid(path))
			return;

		FSplinePointInfo* splineInfo = path->IsNearSplinePoint(this->TravelledDistanceOnPath, 20);
		if (splineInfo != NULL)
		{
			if (splineInfo->bPauseAtPoint)
			{
				splineInfo->bHasBeenVisited = true;
				this->K2_PauseAtLocation();
			}
		}
	}
}

void AOrbGroup::AdjustSpeed() 
{
	if (this->MovementSpeed == OrbSpeedLevel[0])
		this->MovementSpeed = OrbSpeedLevel[2];
	else if (this->MovementSpeed == OrbSpeedLevel[2])
		this->MovementSpeed = OrbSpeedLevel[1];
	else if (this->MovementSpeed == OrbSpeedLevel[1])
		this->MovementSpeed = OrbSpeedLevel[0];

	//UE_LOG(LogTemp, Log, TEXT("Movement Speed: %f"), this->MovementSpeed);
}

///////////////////////////////////////////////////////////////////////////
// OrbGroup Blueprint Methods
void AOrbGroup::K2_ChangeColor(FColor color)
{
	this->ChangeColor(color);
}

void AOrbGroup::K2_StartMoveToTargetWithoutLocation(AActor* target, bool bAttachToTargetAtEnd)
{
	this->StartMoveToTarget(target, bAttachToTargetAtEnd);
}
void AOrbGroup::K2_StartMoveToTargetWithLocation(AActor* target, FVector location, bool bAttachToTargetAtEnd)
{
	this->StartMoveToTarget(target, location, bAttachToTargetAtEnd);
}

void AOrbGroup::K3_StartMoveToTargetWithLocation(FVector location, bool bAttachToTargetAtEnd)
{
	this->StartMoveToTarget(NULL, location, bAttachToTargetAtEnd);
}

void AOrbGroup::K2_PauseAtLocation()
{
	this->bIsPaused = true;
}

void AOrbGroup::K2_ResumeFromLocation()
{
	this->bIsPaused = false;
}

///////////////////////////////////////////////////////////////////////////
// Static Methods

UStaticMeshSocket* AOrbGroup::GetStaticMeshSocket(UStaticMesh* StaticMesh, const FName SocketName)
{
	UStaticMeshSocket* socket = StaticMesh->FindSocket(SocketName);
	return socket;
}
