// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/OrbTriggerComponents/OrbTriggerComponent.h"
#include "Actors/Orbs/OrbGroup.h"
#include "GameFramework/Actor.h"
#include "OrbInteractable.generated.h"

UCLASS()
class ORIGAMI_API AOrbInteractable : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AOrbInteractable();

	// The component which is capable for orb interactions
	UPROPERTY(EditAnywhere)
		UOrbTriggerComponent* TriggerComponent;

	// Orbs might interact with this actor
	virtual void TriggerOrbInteraction(AOrbGroup* IncomingOrbs);
};
