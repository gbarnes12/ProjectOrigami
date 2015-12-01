// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ClimbingPlant.generated.h"

UCLASS()
class ORIGAMI_API AClimbingPlant : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AClimbingPlant();

	// Mesh Component
	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* Mesh;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
};
