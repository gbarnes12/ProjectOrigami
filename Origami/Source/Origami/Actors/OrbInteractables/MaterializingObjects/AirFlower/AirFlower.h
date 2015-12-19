// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/AirFlowComponent.h"
#include "Actors/OrbInteractables/MaterializingObjects/MaterializingObject.h"
#include "AirFlower.generated.h"

UCLASS()
class ORIGAMI_API AAirFlower : public AMaterializingObject
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

	// The particle system visualising the airflow
	UPROPERTY(EditAnywhere)
		UParticleSystemComponent* WindEffect;
};
