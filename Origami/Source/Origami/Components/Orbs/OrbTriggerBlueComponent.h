// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/Orbs/OrbTriggerComponent.h"
#include "OrbTriggerBlueComponent.generated.h"


UCLASS()
class ORIGAMI_API UOrbTriggerBlueComponent : public UOrbTriggerComponent
{
	GENERATED_BODY()

public:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Trigger function called by incoming orbs
	virtual bool TriggerOrbInteraction(AOrbGroup* IncomingOrbs) override;

	// Function to check for ability to pull the trigger
	virtual bool IsAbleToTrigger(AOrbGroup* IncomingOrbs) override;
};
