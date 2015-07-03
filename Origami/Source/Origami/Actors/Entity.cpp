// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"

#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "OrigamiGameMode.h"
#include "Entity.h"


// Sets default values
AEntity::AEntity()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set the tag we need in this case Entity!
	this->Tags.Add(TEXT("Entity"));
	
	// create a custom root scene component (this seems to cause less errors somehow
	USceneComponent* rootSceneComp = CreateAbstractDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	if (IsValid(rootSceneComp))
	{
		rootSceneComp->RelativeLocation = FVector::ZeroVector;
		this->RootComponent = rootSceneComp;
	}

	// Create trigger box 
	// This can be used in order to check whether the player is within reach of the entity or not!
	this->AimBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AimBox"));
	if (IsValid(this->AimBox))
	{
		this->AimBox->SetRelativeLocation(FVector::ZeroVector);
		this->AimBox->AttachTo(this->RootComponent);
	}
}

///////////////////////////////////////////////////////////////////////////
// UE4 Native Event

// Called when the game starts or when spawned
void AEntity::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEntity::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
}

///////////////////////////////////////////////////////////////////////////
// Gameplay

void AEntity::EnterInteractionRange(AOrigamiCharacter* player, FVector collisionPoint)
{

}

void AEntity::LeaveInteractionRange(AOrigamiCharacter* player)
{

}

void AEntity::Interact(AOrigamiCharacter* character)
{

}

void AEntity::Dismantle()
{

}

void AEntity::Mantle()
{

}

///////////////////////////////////////////////////////////////////////////
// Blueprint Utility Methods

AActor* AEntity::NewActorFromString(AActor* Actor, const FString Path, const FString Name)
{
	if (!Actor)
		return nullptr;

	FStringAssetReference fileReference(Path + Name);
	
	AOrigamiGameMode* mode = (AOrigamiGameMode*)Actor->GetWorld()->GetAuthGameMode();
	if (!IsValid(mode))
		return nullptr;

	UBlueprint* objectFinder = *mode->ConstructorStatics.Objects.Find(FName(*fileReference.ToString()));
	if (!objectFinder)
		return nullptr;

	UWorld* world = Actor->GetWorld();
	AActor* actor = world->SpawnActor<AActor>(objectFinder->GeneratedClass);

	if (IsValid(actor))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Couldn't create entity %s!"), *Name);
		return actor;
	}

	UE_LOG(LogTemp, Warning, TEXT("Couldn't create blueprint %s!"), *Name);

	return nullptr;
}

AEntity* AEntity::NewEntityFromString(AActor* Actor, const FString Path, const FString Name)
{
	TArray<UObject*> LoadedObjects;

	if (!Actor)
		return nullptr;

	FStringAssetReference  fileReference = "Blueprint'" + Path + Name + "'";

	UObject* loadedObject = StaticLoadObject(UObject::StaticClass(), nullptr, *fileReference.ToString());
	if (!IsValid(loadedObject))
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't load file %s!"), *Name);
		return nullptr;
	}

	UBlueprint* entityBp = Cast<UBlueprint>(fileReference.ResolveObject());
	if (IsValid(entityBp))
	{
		UWorld* world = Actor->GetWorld();
		AEntity* entity = world->SpawnActor<AEntity>(entityBp->GeneratedClass);

		if (IsValid(entity))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Couldn't create entity %s!"), *Name);
			return entity;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Couldn't create blueprint %s!"), *Name);

	return nullptr;
}
