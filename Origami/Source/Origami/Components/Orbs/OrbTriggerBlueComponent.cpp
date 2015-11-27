// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "OrbTriggerBlueComponent.h"


// Called when the game starts
void UOrbTriggerBlueComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UOrbTriggerBlueComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// Trigger function called by incoming orbs
bool UOrbTriggerBlueComponent::TriggerOrbInteraction(AOrbGroup* IncomingOrbs)
{
	if (IsAbleToTrigger(IncomingOrbs))
		return true;

	return false;
}


// Function to check for ability to pull the trigger
bool UOrbTriggerBlueComponent::IsAbleToTrigger(AOrbGroup* IncomingOrbs)
{
	if (Super::TriggerOrbInteraction(IncomingOrbs) && IncomingOrbs->OrbColor == FColor::Blue)
		return true;

	return false;
}