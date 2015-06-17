// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "Entity.h"


// Sets default values
AEntity::AEntity()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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


AEntity* AEntity::NewEntityFromString(AActor* Actor, const FString Path, const FString Name)
{
	TArray<UObject*> LoadedObjects;

	if (!Actor)
		return nullptr;

	FStringAssetReference  fileReference = "Blueprint'" + Path + Name + "'";

	/*UObject* loadedObject = StaticLoadObject(UObject::StaticClass(), nullptr, *fileReference.ToString());
	if (!IsValid(loadedObject))
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't load file %s!"), *Name);
		return nullptr;
	}*/

	UBlueprint* entityBp = Cast<UBlueprint>(fileReference.ResolveObject());
	if (IsValid(entityBp))
	{
		FActorSpawnParameters fp;
		fp.bAllowDuringConstructionScript = true;

		UWorld* world = Actor->GetWorld();
		AEntity* entity = world->SpawnActor<AEntity>(entityBp->GeneratedClass, fp);

		if (IsValid(entity))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Couldn't create entity %s!"), *Name);
			return entity;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Couldn't create blueprint %s!"), *Name);

	return nullptr;
}
