// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/OrbInteractables/OrbInteractable.h"
#include "MaterializingObject.generated.h"

UCLASS()
class ORIGAMI_API AMaterializingObject : public AOrbInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMaterializingObject();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Orbs might interact with this actor
	virtual void TriggerOrbInteraction(AOrbGroup* IncomingOrbs) override;
};