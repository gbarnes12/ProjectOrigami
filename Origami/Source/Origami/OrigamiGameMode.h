// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "OrigamiGameMode.generated.h"

struct FConstructorStatics
{
public:
	class TMap<FName, UBlueprint*> Objects;
	void Initialize();
};

UCLASS(minimalapi)
class AOrigamiGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	struct FConstructorStatics ConstructorStatics;
	
	AOrigamiGameMode(const FObjectInitializer& ObjectInitializer);

};



