// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/OrbInteractables/OrbInteractable.h"
#include "AttackableCreature.generated.h"

UCLASS()
class ORIGAMI_API AAttackableCreature : public AOrbInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAttackableCreature();

	// Orbs might interact with this actor
	virtual void TriggerOrbInteraction(AOrbGroup* IncomingOrbs) override;
};