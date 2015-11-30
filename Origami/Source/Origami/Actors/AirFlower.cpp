// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "AirFlower.h"
#include "Actors/Entity.h"
#include "Components/AirFlowComponent.h"

///////////////////////////////////////////////////////////////////////////
// AirFlower
AAirFlower::AAirFlower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a new SkeletalMeshComponent
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));

	// Create a new AirFlow-Area
	AirFlow = CreateDefaultSubobject<UAirFlowComponent>(TEXT("AirFlowComponent"));

	// Find this mesh in the source folders
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CubeVisualAsset(TEXT("/Game/Origami/Objects/AirFlower/Controller/AirFlowerSkeletalMesh.AirFlowerSkeletalMesh"));
	
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
}

///////////////////////////////////////////////////////////////////////////
// Gameplay
void AAirFlower::EnterInteractionRange(AOrigamiCharacter* player, FVector collisionPoint)
{

}

void AAirFlower::LeaveInteractionRange(AOrigamiCharacter* player)
{

}


void AAirFlower::Interact(AOrigamiCharacter* player)
{

}