// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AirFlower.h"
#include "Actors/OrbInteractable.h"
#include "AirFlowerGroup.generated.h"

UCLASS()
class ORIGAMI_API AAirFlowerGroup : public AOrbInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAirFlowerGroup();

	// A collection of all by the group included AirFlowers
	UPROPERTY(EditAnywhere, Category = SubElements)
		TArray<AAirFlower*> AirFlowers;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Orbs might interact with this actor
	virtual void TriggerOrbInteraction(AOrbGroup* IncomingOrbs) override;
};