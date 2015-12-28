// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "ReapingHook.h"
#include "ReapingHookAIController.h"


// Sets default values
AReapingHook::AReapingHook()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a new SkeletalMeshComponent
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));

	// Find this mesh in the source folders
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshFinder(TEXT("/Game/Origami/Objects/ClimbingPlant/Controller/ClimbingPlantSkeletalMesh.ClimbingPlantSkeletalMesh"));

	// If it is found, set it as the mesh
	if (MeshFinder.Succeeded())
	{
		Mesh->SetSkeletalMesh(MeshFinder.Object);
	}

	// The reaping hook gets controlled by the reaping hook controller
	AIControllerClass = AReapingHookAIController::StaticClass();

	// Either when placed in the world or when spawned
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called every frame
void AReapingHook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
	//                                   //
	//            AI things              //
	//                                   //
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
}

// Orbs might interact with this actor
void AReapingHook::TriggerOrbInteraction(AOrbFlock* IncomingOrbs)
{
	Super::TriggerOrbInteraction(IncomingOrbs);

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
	//                                   //
	//    Maybe reaction to being hit    //
	//                                   //
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
}