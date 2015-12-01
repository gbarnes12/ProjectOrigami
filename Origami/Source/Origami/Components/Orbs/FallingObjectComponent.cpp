// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "FallingObjectComponent.h"


// Called when the game starts
void UFallingObjectComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UFallingObjectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// Trigger function called by incoming orbs
bool UFallingObjectComponent::TriggerOrbInteraction(AOrbGroup* IncomingOrbs)
{
	if (IsAbleToTrigger(IncomingOrbs))
		return true;

	return false;
}


// Function to check for ability to pull the trigger
bool UFallingObjectComponent::IsAbleToTrigger(AOrbGroup* IncomingOrbs)
{
	return Super::IsAbleToTrigger(IncomingOrbs);
}