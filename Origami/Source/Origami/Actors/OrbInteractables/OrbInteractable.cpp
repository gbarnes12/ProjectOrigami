// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "OrbInteractable.h"


// Sets default values
AOrbInteractable::AOrbInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set a default trigger component for any unspecialised objects
	TriggerComponent = CreateDefaultSubobject<UOrbTriggerComponent>(TEXT("OrbTriggerComponent"));
}

// Orbs might interact with this actor
void AOrbInteractable::TriggerOrbInteraction(AOrbGroup* IncomingOrbs)
{

}