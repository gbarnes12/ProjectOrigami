// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "DissolveComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ORIGAMI_API UDissolveComponent : public UActorComponent
{
	GENERATED_BODY()

	
	UPROPERTY()
	UMaterial* BaseMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* MaterialInstance;

	// Private values
	float DissolveValue = 1.0f;
	
	// Change to dissolve/restore (visible in editor for testing purposes)
	UPROPERTY(EditAnywhere)
		bool bShouldDissolve = true;
	
	// How long the dissolving/restoring should take (in seconds)
	UPROPERTY(EditAnywhere)
		float DissolveTime = 2.0f;

public:
	// Sets default values for this component's properties
	UDissolveComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Inline functions
	void Dissolve();
	void Assemble();

	bool IsFullyDissolved();
	bool IsFullyAssembled();

	// Getters/Setters
	float GetDissolveTime() { return DissolveTime; }
	void SetDissolveTime(float Seconds) { DissolveTime = Seconds; }
};
