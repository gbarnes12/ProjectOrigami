// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsAsset.h"
#include "Components/OrbTriggerComponents/Blue/OrbTriggerBlueComponent.h"
#include "ClimbingPlant.h"


// Sets default values
AClimbingPlant::AClimbingPlant()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a new SkeletalMeshComponent
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));

	// Find this mesh in the source folders
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshFinder(TEXT("/Game/Origami/Objects/ClimbingPlant/Controller/ClimbingPlantSkeletalMesh.ClimbingPlantSkeletalMesh"));
//	static ConstructorHelpers::FObjectFinder<UAnimationAsset> AnimationFinder(TEXT("/Game/Origami/Objects/AirFlower/Animations/AirFlowerPuffingDefault_Loop.AirFlowerPuffingDefault_Loop"));
	static ConstructorHelpers::FObjectFinder<UPhysicsAsset> PhysicsFinder(TEXT("/Game/Origami/Objects/ClimbingPlant/Controller/ClimbingPlantSkeletalMesh_PhysicsAsset.ClimbingPlantSkeletalMesh_PhysicsAsset"));


	// If it is found, set it as the mesh
	if (MeshFinder.Succeeded() &&  PhysicsFinder.Succeeded())
	{
		Mesh->SetRelativeLocation(FVector::ZeroVector);
		Mesh->SetSkeletalMesh(MeshFinder.Object);
		Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);

		//Mesh->AnimationData.AnimToPlay = AnimationFinder.Object;
		Mesh->SetPhysicsAsset(PhysicsFinder.Object);
		Mesh->SetCollisionProfileName(TEXT("BlockAll"));
		Mesh->AttachTo(this->RootComponent);
	}

	// Overwrite the default trigger component to enable the color check
	TriggerComponent = CreateDefaultSubobject<UOrbTriggerComponent>(TEXT("OrbTriggerBlueComponent"));
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
void AClimbingPlant::TriggerOrbInteraction(AOrbFlock* IncomingOrbs)
{
	Super::TriggerOrbInteraction(IncomingOrbs);

	// On interaction just set the platform ascending
	bIsAscending = true;
}