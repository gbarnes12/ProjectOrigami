// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/AirFlower.h"
#include "Actors/Entity.h"
#include "AirFlowerGroup.generated.h"

/**
*
*/
UCLASS()
class ORIGAMI_API AAirFlowerGroup : public AEntity
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

	// Can be called whenever the player interacts with this entity (via Key Press: Interact).
	virtual void Interact(AOrigamiCharacter* player) override;

	// Is called whenever the player is within interaction range and this entity is hit by his ray.
	virtual void EnterInteractionRange(AOrigamiCharacter* player, FVector collisionPoint) override;
	virtual void LeaveInteractionRange(AOrigamiCharacter* player) override;
};