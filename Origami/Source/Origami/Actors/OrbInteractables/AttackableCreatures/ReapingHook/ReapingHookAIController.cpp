// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "ReapingHookAIController.h"


// Sets default values
AReapingHookAIController::AReapingHookAIController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a new BehaviourTree
	BehaviourTree = CreateDefaultSubobject<UReapingHookBehaviourTree>(TEXT("BehaviourTree"));

	// And run it
	RunBehaviorTree(BehaviourTree);

	// Create a new Blackboard
	BlackboardData = CreateDefaultSubobject<UReapingHookBlackboard>(TEXT("Blackboard"));

	// And use it
	UseBlackboard(BlackboardData, Blackboard);
}