// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "Actors/Orbs/OrbFlock.h"
#include "OrbTriggerRedComponent.h"


// Function to check for ability to pull the trigger
bool UOrbTriggerRedComponent::IsAbleToTrigger(AOrbFlock* IncomingOrbs)
{
	return IncomingOrbs->Color == FColor::Red;
}