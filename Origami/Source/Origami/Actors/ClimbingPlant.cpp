// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "ClimbingPlant.h"


// Sets default values
AClimbingPlant::AClimbingPlant()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a new SkeletalMeshComponent
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));

	// Find this mesh in the source folders
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CubeVisualAsset(TEXT("/Game/Origami/Objects/ClimbingPlant/Controller/ClimbingPlantSkeletalMesh.ClimbingPlantSkeletalMesh"));

	// If it is found, set it as the mesh
	if (CubeVisualAsset.Succeeded())
	{
		Mesh->SetSkeletalMesh(CubeVisualAsset.Object);
	}
}

// Called when the game starts or when spawned
void AClimbingPlant::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AClimbingPlant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

