// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/OrbTriggerComponents/OrbTriggerComponent.h"
#include "GameFramework/Pawn.h"
#include "OrbInteractable.generated.h"

UCLASS()
class ORIGAMI_API AOrbInteractable : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock")
	bool bIsUsable;

public:
	// Sets default values for this actor's properties
	AOrbInteractable();


	void BeginPlay() override;

	// The component which is capable for orb interactions
	UPROPERTY(EditAnywhere)
		UOrbTriggerComponent* TriggerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock")
		FVector AttachmentPoint;

	// Orbs might interact with this actor
	UFUNCTION(BlueprintCallable, Category = "Flock")
	virtual void TriggerOrbInteraction(AOrbFlock* IncomingOrbs);

	UFUNCTION(BlueprintCallable, Category = "Flock")
	virtual bool GetIsUsable();
};
