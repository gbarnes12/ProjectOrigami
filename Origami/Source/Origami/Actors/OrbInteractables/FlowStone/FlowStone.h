// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/OrbInteractables/OrbInteractable.h"
#include "FlowStone.generated.h"

UCLASS()
class ORIGAMI_API AFlowStone : public AOrbInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlowStone();

	// Mesh Component
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;

	// The z-value where the object should stop falling
	UPROPERTY(EditAnywhere, Category = FallingObject)
		float GravityStop = 0;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Orbs might interact with this actor
	virtual void TriggerOrbInteraction(AOrbFlock* IncomingOrbs) override;
};
