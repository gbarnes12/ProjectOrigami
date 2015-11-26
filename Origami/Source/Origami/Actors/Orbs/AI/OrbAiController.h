// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"

#include "OrbAiController.generated.h"

/**
 * 
 */
UCLASS()
class ORIGAMI_API AOrbAiController : public AAIController
{
	GENERATED_BODY()

public:

	UBlackboardComponent* BlackboardComponent;


public:

	AOrbAiController(const FObjectInitializer& ObjectInitializer);
	
	void Possess(APawn* Pawn);
};
