// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "OrbTriggerRedComponent.h"


// Called when the game starts
void UOrbTriggerRedComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UOrbTriggerRedComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// Trigger function called by incoming orbs
bool UOrbTriggerRedComponent::TriggerOrbInteraction(AOrbGroup* IncomingOrbs)
{
	if (IsAbleToTrigger(IncomingOrbs))
		return true;

	return false;
}


// Function to check for ability to pull the trigger
bool UOrbTriggerRedComponent::IsAbleToTrigger(AOrbGroup* IncomingOrbs)
{
	if (Super::TriggerOrbInteraction(IncomingOrbs) && IncomingOrbs->OrbColor == FColor::Red)
		return true;

	return false;
}