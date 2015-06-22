#include "Origami.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Actors/Characters/Player/OrigamiCharacter.h"
#include "Actors/Entities/Cocoon.h"
#include "Actors/Entity.h"
#include "OrbGroup.h"

///////////////////////////////////////////////////////////////////////////
// AOrbGroup

AOrbGroup::AOrbGroup(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->TravelledDistanceOnPath = 0.0f;
	this->Mode = EOrbMode::Swarm;
	this->OrbCount = 20;
	this->OrbColor = FColor::White;
	this->OrbSpawnBoxExtents = 20.0f;

	this->bIsGenerated = false;
	this->OrbMeshFileName = TEXT("StaticMesh'/Game/Origami/Meshes/OrbMesh.OrbMesh'");
	this->OrbPath = NULL;

	// The socket functions as 
	this->Socket = NULL;

	// Root Scene Component
	this->RootSceneComponent = CreateAbstractDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	this->RootSceneComponent->RelativeLocation = FVector::ZeroVector;
	this->RootComponent = this->RootSceneComponent;

	if (this->RootSceneComponent)
	{
		// Spline Component
		this->OrbPath = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
		if (this->OrbPath)
		{
			this->OrbPath->AttachTo(this->RootComponent);
			this->OrbPath->SetRelativeLocation(FVector::ZeroVector);
		}
	}
}

///////////////////////////////////////////////////////////////////////////
// UE4 Native Events

void AOrbGroup::BeginPlay()
{
	Super::BeginPlay();

	this->MovementSpeed = OrbSpeedLevel[0];

	// retrieve the player and set him 
	// this is just temporarily!
	//APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	//AttachSocket(playerController->GetPawn());

	// GenerateOrbs only at runtime 
	// if we do this within the constructor the editor becomes slow as fuck somehow?!
	this->GenerateOrbs();

	/*Start the timer that will adjust the speed over time*/
	GetWorldTimerManager().ClearTimer(AdjustSpeedTimerHandle);
	GetWorldTimerManager().SetTimer(AdjustSpeedTimerHandle, this, &AOrbGroup::AdjustSpeed, 5.0f, true);
}


void AOrbGroup::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	//UKismetSystemLibrary::DrawDebugBox(GetWorld(), this->RootComponent->GetComponentLocation(), FVector(200), FLinearColor::Red, FRotator::ZeroRotator, 0.1f);
	//UKismetSystemLibrary::DrawDebugBox(GetWorld(), this->BoxSceneComponent->GetComponentLocation(), this->BoxSceneComponent->GetUnscaledBoxExtent(), FLinearColor::Blue, FRotator::ZeroRotator, 0.1f);

	if (!this->OrbsSceneComponent->IsValidLowLevelFast())
		return;

	if (this->AttachedType == EActorType::None)
		return;

	this->FollowPath(deltaSeconds);
}


///////////////////////////////////////////////////////////////////////////
// Construction

void AOrbGroup::GenerateOrbs()
{
	// take care that we do not recreate the 
	// orbs!
	if (this->bIsGenerated)
		return;

	this->OrbsSceneComponent = NewObject<USceneComponent>(this, USceneComponent::StaticClass(), TEXT("Orbs"));
	if (!this->OrbsSceneComponent)
		return;

	const FVector meshScale = FVector(0.20f, 0.20f, 0.20f);

	this->OrbsSceneComponent->SetRelativeLocation(FVector::ZeroVector);
	this->OrbsSceneComponent->AttachTo(this->RootComponent);
	this->OrbsSceneComponent->RegisterComponent();


	UPointLightComponent* light = NewObject<UPointLightComponent>(this, UPointLightComponent::StaticClass(), TEXT("PointLight"));
	if (light)
	{
		light->SetRelativeRotation(FRotator::ZeroRotator);
		light->SetLightColor(FLinearColor::White);
		light->SetAttenuationRadius(180.0f);
		light->SetSourceRadius(400.0f);
		light->AttachTo(OrbsSceneComponent);
		light->RegisterComponent();
	}

	// load static mesh
	this->OrbMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *this->OrbMeshFileName));
	if (!this->OrbMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't load orb mesh with filename %s"), *this->OrbMeshFileName);
		return;
	}

	FString particleSystemName = TEXT("StaticMesh'/Game/Origami/Particles/Ps_Orb_AnimTrail.Ps_Orb_AnimTrail'");

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
	const FBox spawnBox = FBox::BuildAABB(this->OrbsSceneComponent->GetRelativeTransform().GetLocation(), FVector(this->OrbSpawnBoxExtents));

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
			meshComp->AttachTo(this->OrbsSceneComponent);

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
// Socket Methods

void AOrbGroup::AttachSocket(AActor* socket)
{
	if (!IsValid(socket))
		return;
	
	this->AttachedType = EActorType::None;

	if (socket->ActorHasTag(TEXT("Player"))) 
	{
		this->AttachedType = EActorType::Player;
		this->Socket = socket;
		const AOrigamiCharacter* player = Cast<AOrigamiCharacter>(this->Socket);
		this->OrbPath = player->OrbPath;
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
	}

	this->K2_AttachRootComponentToActor(this->Socket, NAME_None, EAttachLocation::Type::KeepWorldPosition);
}

void AOrbGroup::DetachFromSocket() 
{
	if (this->Socket == NULL)
		return;
	
	this->DetachRootComponentFromParent(true);
	this->Socket = NULL;
	this->AttachedType = EActorType::None;
}


///////////////////////////////////////////////////////////////////////////
// Simulation Methods

void AOrbGroup::SimulateSwarm(float deltaSeconds)
{
	if (!this->Socket)
		return;

//	float speedAndTime = 30.0f * deltaSeconds;
	//this->CurrentRotation = this->CurrentRotation + speedAndTime;
	//FVector currentLocation = this->OrbsSceneComponent->GetComponentLocation();

	const FVector socketLocation = this->Socket->GetActorLocation();
	//FVector NewActorLocation = OrbsSceneComponent->GetComponentLocation().RotateAngleAxis(10.0f  * deltaSeconds, this->Socket->GetActorUpVector());
	//NewActorLocation = NewActorLocation + socketLocation;


	this->SetActorLocation(socketLocation);
	//this->OrbsSceneComponent->SetWorldLocation(socketLocation);
	//this->OrbsSceneComponent->SetWorldRotation(NewActorLocation.Rotation());
}

void AOrbGroup::FollowPath(float deltaSeconds) 
{
	if (!this->OrbPath)
		return;

	float pathDistance = this->OrbPath->GetSplineLength();
	this->TravelledDistanceOnPath = FMath::FInterpConstantTo(this->TravelledDistanceOnPath, pathDistance, deltaSeconds, this->MovementSpeed);

	if (this->TravelledDistanceOnPath >= pathDistance)
		this->TravelledDistanceOnPath = 0.0f;

	const FVector location = this->OrbPath->GetWorldLocationAtDistanceAlongSpline(TravelledDistanceOnPath);
	const FRotator rotation = this->OrbPath->GetWorldRotationAtDistanceAlongSpline(TravelledDistanceOnPath);

	this->OrbsSceneComponent->SetWorldLocationAndRotation(location, rotation, true);
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
// Static Methods

UStaticMeshSocket* AOrbGroup::GetStaticMeshSocket(UStaticMesh* StaticMesh, const FName SocketName)
{
	UStaticMeshSocket* socket = StaticMesh->FindSocket(SocketName);
	return socket;
}
