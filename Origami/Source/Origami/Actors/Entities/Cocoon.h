// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/Entity.h"
#include "Cocoon.generated.h"

/**
 * 
 */
UCLASS()
class ORIGAMI_API ACocoon : public AEntity
{
	GENERATED_BODY()

public:
	ACocoon();
	virtual void BeginPlay() override;

	USplineComponent* GetOrbPath() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Visual, meta = (AllowPrivateAccess = "true"))
	class USplineComponent* OrbPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Visual, meta = (AllowPrivateAccess = "true"))
	class AOrbGroup* Orbs;
};
