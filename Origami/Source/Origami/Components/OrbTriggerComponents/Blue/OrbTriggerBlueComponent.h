// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/OrbTriggerComponents/OrbTriggerComponent.h"
#include "OrbTriggerBlueComponent.generated.h"


UCLASS()
class ORIGAMI_API UOrbTriggerBlueComponent : public UOrbTriggerComponent
{
	GENERATED_BODY()

public:
	// Function to check for ability to pull the trigger
	virtual bool IsAbleToTrigger(AOrbFlock* IncomingOrbs) override;
};
