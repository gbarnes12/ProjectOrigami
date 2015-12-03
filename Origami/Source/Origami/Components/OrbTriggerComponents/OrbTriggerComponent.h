// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/Orbs/OrbGroup.h"
#include "Components/ActorComponent.h"
#include "OrbTriggerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ORIGAMI_API UOrbTriggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOrbTriggerComponent();

	// Trigger function called by incoming orbs
	virtual bool TriggerOrbInteraction(AOrbGroup* IncomingOrbs);

	// Function to check for ability to pull the trigger
	virtual bool IsAbleToTrigger(AOrbGroup* IncomingOrbs);
};
