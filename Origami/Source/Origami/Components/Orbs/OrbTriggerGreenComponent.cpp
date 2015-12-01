// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "OrbTriggerGreenComponent.h"


// Called when the game starts
void UOrbTriggerGreenComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UOrbTriggerGreenComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// Trigger function called by incoming orbs
bool UOrbTriggerGreenComponent::TriggerOrbInteraction(AOrbGroup* IncomingOrbs)
{
	if (IsAbleToTrigger(IncomingOrbs))
		return Super::TriggerOrbInteraction(IncomingOrbs);

	return false;
}

// Function to check for ability to pull the trigger
bool UOrbTriggerGreenComponent::IsAbleToTrigger(AOrbGroup* IncomingOrbs)
{
	return Super::IsAbleToTrigger(IncomingOrbs) && IncomingOrbs->OrbColor == FColor::Green;
}