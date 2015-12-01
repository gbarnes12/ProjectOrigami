// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "AirFlower.h"
#include "Components/AirFlowComponent.h"


// Sets default values
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

// Called when the game starts or when spawned
void AAirFlower::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAirFlower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Orbs might interact with this actor
void AAirFlower::TriggerOrbInteraction(AOrbGroup* IncomingOrbs)
{

}