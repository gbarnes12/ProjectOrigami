// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "FlowStone.h"
#include "Components/OrbTriggerComponents/Blue/OrbTriggerBlueComponent.h"


// Sets default values
AFlowStone::AFlowStone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a new SkeletalMeshComponent
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	// Find this mesh in the source folders
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Origami/Meshes/test.test"));

	// If it is found, set it as the mesh
	if (MeshFinder.Succeeded())
	{
		Mesh->SetStaticMesh(MeshFinder.Object);
	}

	// Overwrite the default trigger component to enable the color check
	TriggerComponent = CreateDefaultSubobject<UOrbTriggerComponent>(TEXT("OrbTriggerBlueComponent"));
}

// Called every frame
void AFlowStone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check whether the object is simulating physics and whether it is below the predefined height
	if (Mesh->IsSimulatingPhysics() && GetActorLocation().Z <= GravityStop)
	{
		// Then stop the physics simulation and set the rigid body as static
		Mesh->SetSimulatePhysics(false);
		Mesh->SetEnableGravity(false);
		Mesh->SetMobility(EComponentMobility::Static);
	}
}

// Orbs might interact with this actor
void AFlowStone::TriggerOrbInteraction(AOrbFlock* IncomingOrbs)
{
	Super::TriggerOrbInteraction(IncomingOrbs);

	if (GetActorLocation().Z > GravityStop)
	{
		// Start simulation of physics, gravity and mobility
		Mesh->SetMobility(EComponentMobility::Movable);
		Mesh->SetEnableGravity(true);
		Mesh->SetSimulatePhysics(true);
	}
}