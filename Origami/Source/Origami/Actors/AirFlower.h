// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Orbs might interact with this actor
	void Interact();
};
