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
	virtual void BeginPlay() override;
};
