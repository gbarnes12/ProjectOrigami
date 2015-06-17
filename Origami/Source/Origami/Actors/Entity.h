// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Entity.generated.h"

UCLASS()
class ORIGAMI_API AEntity : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEntity();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	UFUNCTION(BlueprintPure, meta = (DisplayName = "New Entity from string", Keywords = "New Entity from string"), Category = Game)
	static AEntity* NewEntityFromString(AActor* actor, const FString Path, const FString Name);
};
