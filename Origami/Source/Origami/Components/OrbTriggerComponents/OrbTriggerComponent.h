// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "OrbTriggerComponent.generated.h"


class AOrbFlock;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ORIGAMI_API UOrbTriggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOrbTriggerComponent();

	// Trigger function called by incoming orbs
	UFUNCTION(BlueprintCallable, Category = "Flock")
	virtual bool TriggerOrbInteraction(AOrbFlock* IncomingOrbs);

	// Function to check for ability to pull the trigger
	virtual bool IsAbleToTrigger(AOrbFlock* IncomingOrbs);
};
