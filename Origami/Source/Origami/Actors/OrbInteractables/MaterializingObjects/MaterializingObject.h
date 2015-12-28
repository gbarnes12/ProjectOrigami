// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/DissolveComponent.h"
#include "Actors/OrbInteractables/OrbInteractable.h"
#include "MaterializingObject.generated.h"

UCLASS()
class ORIGAMI_API AMaterializingObject : public AOrbInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMaterializingObject();

	// How long should it take to fade out again? (in seconds)
	UPROPERTY(EditAnywhere, Category = DissolveComponent)
		float TimeToFadeOut = 120;

	// The component which is capable fading in and out the object
	UPROPERTY(EditAnywhere, Category = DissolveComponent)
		UDissolveComponent* DissolveComponent;

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Orbs might interact with this actor
	virtual void TriggerOrbInteraction(AOrbFlock* IncomingOrbs) override;

private:
	// How long is the platform faded in yet?
	float FadeOutTimer = 0;

	AOrbFlock* Flock = NULL;
};