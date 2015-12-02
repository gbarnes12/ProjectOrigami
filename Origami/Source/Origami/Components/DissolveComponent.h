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

	UPROPERTY(EditAnywhere)
	bool bDissolve = false;
	float DissolveValue = 0.0f;

public:	
	// Sets default values for this component's properties
	UDissolveComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void SetDissolve(bool State);
};
