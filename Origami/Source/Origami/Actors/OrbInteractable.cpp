// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "OrbInteractable.h"


// Sets default values
AOrbInteractable::AOrbInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a new OrbTriggerComponent
	TriggerComponent = CreateDefaultSubobject<UOrbTriggerComponent>(TEXT("OrbTriggerComponent"));
}

// Called when the game starts or when spawned
void AOrbInteractable::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AOrbInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Orbs might interact with this actor
void AOrbInteractable::TriggerOrbInteraction(AOrbGroup* IncomingOrbs)
{

}