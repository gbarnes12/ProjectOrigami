// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/OrbInteractables/OrbInteractable.h"
#include "ClimbingPlant.generated.h"

UCLASS()
class ORIGAMI_API AClimbingPlant : public AOrbInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AClimbingPlant();

	// Mesh Component
	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* Mesh;

	// How long should it take to unfold the platform? (in seconds)
	UPROPERTY(EditAnywhere, Category = ClimbingPlant)
		float AscensionSpeed = 3;

	// How long should it take to descend again? (in seconds)
	UPROPERTY(EditAnywhere, Category = ClimbingPlant)
		float TimeToDescend = 120;
	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Orbs might interact with this actor
	virtual void TriggerOrbInteraction(AOrbFlock* IncomingOrbs) override;

private:
	// Is the platform currently ascending?
	bool bIsAscending = false;

	// Is the platform currently descending?
	bool bIsDescending = false;

	// How long is the platform ascended yet?
	float DescensionTimer = 0;
};
