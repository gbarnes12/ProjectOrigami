// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "FlowStone.h"
#include "Components/Orbs/OrbTriggerBlueComponent.h"


// Sets default values
AFlowStone::AFlowStone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a new SkeletalMeshComponent
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	// The component triggered by the orbs has to be adjusted
	TriggerComponent = CreateDefaultSubobject<UOrbTriggerBlueComponent>(TEXT("OrbTriggerBlueComponent"));
}

// Called when the game starts or when spawned
void AFlowStone::BeginPlay()
{
	Super::BeginPlay();
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
void AFlowStone::TriggerOrbInteraction(AOrbGroup* IncomingOrbs)
{
	if (GetActorLocation().Z > GravityStop)
	{
		// Start simulation of physics, gravity and mobility
		Mesh->SetMobility(EComponentMobility::Movable);
		Mesh->SetEnableGravity(true);
		Mesh->SetSimulatePhysics(true);
	}
}