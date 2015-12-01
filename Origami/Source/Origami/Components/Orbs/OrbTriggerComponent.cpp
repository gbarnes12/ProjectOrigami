// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "OrbTriggerComponent.h"
#include "Actors/OrbInteractable.h"


// Sets default values for this component's properties
UOrbTriggerComponent::UOrbTriggerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UOrbTriggerComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UOrbTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// Trigger function called by incoming orbs
bool UOrbTriggerComponent::TriggerOrbInteraction(AOrbGroup* IncomingOrbs)
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
bool UOrbTriggerComponent::IsAbleToTrigger(AOrbGroup* IncomingOrbs)
{
	return true;
}