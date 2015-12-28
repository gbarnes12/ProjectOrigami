// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "OrbTriggerComponent.h"
#include "Actors/Orbs/OrbFlock.h"
#include "Actors/OrbInteractables/OrbInteractable.h"


// Sets default values for this component's properties
UOrbTriggerComponent::UOrbTriggerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = false;
	PrimaryComponentTick.bCanEverTick = false;
}

// Trigger function called by incoming orbs
bool UOrbTriggerComponent::TriggerOrbInteraction(AOrbFlock* IncomingOrbs)
{
	if (IsAbleToTrigger(IncomingOrbs))
	{
		// If the owner object is orb interactable, call the trigger function
		AActor* owner = GetOwner();
		if (owner->IsA(AOrbInteractable::StaticClass()))
		{
			Cast<AOrbInteractable>(owner)->TriggerOrbInteraction(IncomingOrbs);
			return true;
		}
	}

	return false;
}

// Function to check for ability to pull the trigger
bool UOrbTriggerComponent::IsAbleToTrigger(AOrbFlock* IncomingOrbs)
{
	return true;
}