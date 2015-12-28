// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "Actors/Orbs/OrbFlock.h"
#include "OrbTriggerGreenComponent.h"


// Function to check for ability to pull the trigger
bool UOrbTriggerGreenComponent::IsAbleToTrigger(AOrbFlock* IncomingOrbs)
{
	UE_LOG(LogTemp, Log, TEXT("color: %s"), *IncomingOrbs->Color.ToString())
	return IncomingOrbs->Color == FColor::Green;
}