// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "AirFlower.h"
#include "Actors/Entity.h"

///////////////////////////////////////////////////////////////////////////
// AirFlower
AAirFlower::AAirFlower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a new SkeletalMeshComponent
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));

	// Find this mesh in the source folders
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CubeVisualAsset(TEXT("/Game/Origami/Meshes/AirFlower/air_flower.air_flower"));
	
	// If it is found, set it as the mesh
	if (CubeVisualAsset.Succeeded())
	{
		Mesh->SetSkeletalMesh(CubeVisualAsset.Object);
	}
}

///////////////////////////////////////////////////////////////////////////
// UE4 Native Event
void AAirFlower::BeginPlay()
{
	Super::BeginPlay();

	this->ActionButtonPrompt = AEntity::NewActorFromString(this, TEXT("/Game/Origami/Blueprints/Hud/"), TEXT("Bt_ActionPrompt.Bt_ActionPrompt"), false);
	if (!IsValid(this->ActionButtonPrompt))
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't create a new instance of Bt_ActionPrompt blueprint!"));
		return;
	}

	this->ActionButtonPrompt->AttachRootComponentToActor(this);
	this->ActionButtonPrompt->SetActorHiddenInGame(true);
}

///////////////////////////////////////////////////////////////////////////
// Gameplay
void AAirFlower::EnterInteractionRange(AOrigamiCharacter* player, FVector collisionPoint)
{
	if (!IsValid(this->ActionButtonPrompt) || !IsValid(player))
		return;

	FVector forward = player->GetActorLocation() - collisionPoint;
	this->ActionButtonPrompt->SetActorLocation(collisionPoint + forward * 0.05f);
	this->ActionButtonPrompt->SetActorHiddenInGame(false);
}

void AAirFlower::LeaveInteractionRange(AOrigamiCharacter* player)
{
	this->ActionButtonPrompt->SetActorHiddenInGame(true);
}


void AAirFlower::Interact(AOrigamiCharacter* player)
{

}