// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/OrbTriggerComponents/OrbTriggerComponent.h"
#include "OrbTriggerGreenComponent.generated.h"

class AOrbFlock;

UCLASS()
class ORIGAMI_API UOrbTriggerGreenComponent : public UOrbTriggerComponent
{
	GENERATED_BODY()

public:
	// Function to check for ability to pull the trigger
	virtual bool IsAbleToTrigger(AOrbFlock* IncomingOrbs) override;
};
