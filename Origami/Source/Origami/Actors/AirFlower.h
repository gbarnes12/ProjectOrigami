// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/Characters/Player/OrigamiCharacter.h"
#include "Components/AirFlowComponent.h"
#include "GameFramework/Actor.h"
#include "AirFlower.generated.h"

UCLASS()
class ORIGAMI_API AAirFlower : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAirFlower();

	// Mesh Component
	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* Mesh;

	// The component which is capable for the force addition
	UPROPERTY(EditAnywhere)
		UAirFlowComponent* AirFlow;

	/*UE4 Native Events*/
	virtual void BeginPlay() override;

	// Functions to interact with this object
	void Interact(AOrigamiCharacter* player);
	void EnterInteractionRange(AOrigamiCharacter* player, FVector collisionPoint);
	void LeaveInteractionRange(AOrigamiCharacter* player);

protected:
	class AActor* ActionButtonPrompt;
};
