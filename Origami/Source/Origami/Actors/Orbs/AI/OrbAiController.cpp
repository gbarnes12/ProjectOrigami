// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "../OrbFlock.h"
#include "OrbAiController.h"



AOrbAiController::AOrbAiController(const FObjectInitializer& ObjectInitializer) 
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
}

void AOrbAiController::Possess(APawn * Pawn)
{
	Super::Possess(Pawn);

	AOrbFlock* flock = Cast<AOrbFlock>(Pawn);
	UE_LOG(LogTemp, Log, TEXT("Teeest"));

	if (flock && flock->OrbBehavior && this->BlackboardComponent && flock->BlackboardAsset)
	{
		flock->AiController = this;

		if (this->BlackboardComponent != nullptr)
		{
			this->BlackboardComponent->InitializeBlackboard(*flock->BlackboardAsset);
			this->RunBehaviorTree(flock->OrbBehavior);
			
		}
	}
}
