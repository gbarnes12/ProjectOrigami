// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Origami.h"
#include "OrigamiGameMode.h"
#include "Actors/Characters/Player/OrigamiCharacter.h"

AOrigamiGameMode::AOrigamiGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
