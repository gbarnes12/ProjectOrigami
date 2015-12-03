// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/OrbInteractables/MaterializingObjects/MaterializingObject.h"
#include "Bridge.generated.h"

UCLASS()
class ORIGAMI_API ABridge : public AMaterializingObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABridge();

	// Mesh Component
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;
};
