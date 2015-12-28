// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "EnvQueryGenerator_FreeCocoons.generated.h"

/**
 * 
 */
UCLASS()
class ORIGAMI_API UEnvQueryGenerator_FreeCocoons : public UEnvQueryGenerator
{
	GENERATED_UCLASS_BODY()

	/** max distance of path between point and context */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue SearchRadius;

	/** context */
	UPROPERTY(EditAnywhere, Category = Generator)
		TSubclassOf<UEnvQueryContext> SearchCenter;

	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;
	
	
};
