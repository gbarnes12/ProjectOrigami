// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "AirFlowComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ORIGAMI_API UAirFlowComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAirFlowComponent();

	// The characters mesh, affected by the airflow
	UCharacterMovementComponent* CharacterMovement;

	// The maximum force added to the character being in the airflow (-980 is gravity per kg).
	UPROPERTY(EditAnywhere, Category = AirFlow)
		float FlowStrength = 150000;

	// The maximum distance from the flower, where a force can be added. The further, the less force.
	UPROPERTY(EditAnywhere, Category = AirFlow)
		float MaxDistance = 2000;

	// The maximum angle in which the force is applied, it ends in a cone, where the force decreases in the edge regions.
	UPROPERTY(EditAnywhere, Category = AirFlow)
		float MaxAngle = 45;

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
};
