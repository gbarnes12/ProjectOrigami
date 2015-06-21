// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "Runtime/Engine/Public/EngineUtils.h"
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
	this->TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"), false);
	if (IsValid(this->TriggerBox))
	{
		this->TriggerBox->AttachTo(this->RootComponent);
		this->TriggerBox->RegisterComponent();
	}
}

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


void AEntity::Interact(AOrigamiCharacter* character)
{

}

void AEntity::Dismantle()
{

}

void AEntity::Mantle()
{

}

AActor* AEntity::NewActorFromString(AActor* Actor, const FString Path, const FString Name)
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
		AActor* actor = world->SpawnActor<AActor>(entityBp->GeneratedClass);

		if (IsValid(actor))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Couldn't create entity %s!"), *Name);
			return actor;
		}
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
