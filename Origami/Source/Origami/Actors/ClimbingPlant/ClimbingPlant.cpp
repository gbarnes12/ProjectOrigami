// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "ClimbingPlant.h"
#include "Components/Orbs/OrbTriggerBlueComponent.h"


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

	// The component triggered by the orbs has to be adjusted
	TriggerComponent = CreateDefaultSubobject<UOrbTriggerBlueComponent>(TEXT("OrbTriggerBlueComponent"));
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

	// First we check, whether the platform is ascending
	if (bIsAscending)
	{
		// Adjust the pitch according to the speed set in the settings
		if (GetActorRotation().Pitch <= 90 - DeltaTime / AscensionSpeed * 90)
		{
			AddActorLocalRotation(FRotator(DeltaTime / AscensionSpeed * 90, 0, 0));
		}
		
		// But never go further than 90 degrees
		else if (GetActorRotation().Pitch < 90)
		{
			AddActorLocalRotation(FRotator(90 - GetActorRotation().Pitch, 0, 0));
		}

		// If it is already ascended, stop ascending and set the timer
		else
		{
			bIsAscending = false;
			bIsDescending = false;
			DescensionTimer = TimeToDescend;
		}
	}

	// If it isnt, we only need to act, if it is out of default position
	else if (GetActorRotation().Pitch > 0)
	{
		// Now check, whether the platform is already descending
		if (bIsDescending)
		{
			// Adjust the pitch according to the speed set in the settings
			if (GetActorRotation().Pitch >= DeltaTime / AscensionSpeed * 90)
			{
				AddActorLocalRotation(FRotator(-DeltaTime / AscensionSpeed * 90, 0, 0));
			}

			// But never go further than 0 degrees
			else if (GetActorRotation().Pitch > 0)
			{
				AddActorLocalRotation(FRotator(-GetActorRotation().Pitch, 0, 0));
			}

			// If it is already descended, stop descending
			else
			{
				bIsAscending = false;
				bIsDescending = false;
			}
		}

		// Maybe the time has come to start descending?
		else if (DeltaTime >= DescensionTimer)
		{
			bIsDescending = true;
			DescensionTimer = 0;
		}

		// No? We need to wait and subtract the elapsed time
		else
		{
			DescensionTimer -= DeltaTime;
		}
	}
}

// Orbs might interact with this actor
void AClimbingPlant::TriggerOrbInteraction(AOrbGroup* IncomingOrbs)
{
	// On interaction just set the platform ascending
	bIsAscending = true;
}