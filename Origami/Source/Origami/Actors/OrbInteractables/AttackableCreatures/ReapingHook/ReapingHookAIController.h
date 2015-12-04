// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "ReapingHookBehaviourTree.h"
#include "ReapingHookBlackboard.h"
#include "ReapingHookAIController.generated.h"

UCLASS()
class ORIGAMI_API AReapingHookAIController : public AAIController
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AReapingHookAIController();

	// Behaviour Tree
	UPROPERTY(EditAnywhere)
		UReapingHookBehaviourTree* BehaviourTree;

	// Blackboard
	UPROPERTY(EditAnywhere)
		UReapingHookBlackboard* BlackboardData;
};
