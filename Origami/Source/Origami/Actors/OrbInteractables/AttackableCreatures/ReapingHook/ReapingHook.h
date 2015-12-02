// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/OrbInteractables/AttackableCreatures/AttackableCreature.h"
#include "ReapingHook.generated.h"

UCLASS()
class ORIGAMI_API AReapingHook : public AAttackableCreature
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AReapingHook();

	// Mesh Component
	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* Mesh;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Orbs might interact with this actor
	virtual void TriggerOrbInteraction(AOrbGroup* IncomingOrbs) override;
};
