// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/OrbTriggerComponents/OrbTriggerComponent.h"
#include "OrbTriggerGreenComponent.generated.h"


UCLASS()
class ORIGAMI_API UOrbTriggerGreenComponent : public UOrbTriggerComponent
{
	GENERATED_BODY()

public:
	// Trigger function called by incoming orbs
	virtual bool TriggerOrbInteraction(AOrbGroup* IncomingOrbs) override;

	// Function to check for ability to pull the trigger
	virtual bool IsAbleToTrigger(AOrbGroup* IncomingOrbs) override;
};
