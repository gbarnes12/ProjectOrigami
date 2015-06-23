// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Origami.h"
#include "OrigamiGameMode.h"
#include "Actors/Characters/Player/OrigamiCharacter.h"



void FConstructorStatics::Initialize()
{
	// This is necessary because we cannot reference assets at runtime if they aren't within the build path
	ConstructorHelpers::FObjectFinderOptional<class UBlueprint> Obj_Stc_Cocoon_L_1(TEXT("Blueprint'/Game/Origami/Blueprints/Actors/Bt_Act_Cocoon_L_1.Bt_Act_Cocoon_L_1'"));
	Objects.Add(TEXT("/Game/Origami/Blueprints/Actors/Bt_Act_Cocoon_L_1.Bt_Act_Cocoon_L_1"), Obj_Stc_Cocoon_L_1.Get());

	ConstructorHelpers::FObjectFinderOptional<class UBlueprint> Obj_Stc_Cocoon_M_1(TEXT("Blueprint'/Game/Origami/Blueprints/Actors/Bt_Act_Cocoon_M_1.Bt_Act_Cocoon_M_1'"));
	Objects.Add(TEXT("/Game/Origami/Blueprints/Actors/Bt_Act_Cocoon_M_1.Bt_Act_Cocoon_M_1"), Obj_Stc_Cocoon_M_1.Get());

	ConstructorHelpers::FObjectFinderOptional<class UBlueprint> Obj_Stc_Cocoon_S_1(TEXT("Blueprint'/Game/Origami/Blueprints/Actors/Bt_Act_Cocoon_S_1.Bt_Act_Cocoon_S_1'"));
	Objects.Add(TEXT("/Game/Origami/Blueprints/Actors/Bt_Act_Cocoon_S_1.Obj_Stc_Cocoon_S_1"), Obj_Stc_Cocoon_S_1.Get());

	ConstructorHelpers::FObjectFinderOptional<class UBlueprint> Obj_Stc_Cocoon_S_2(TEXT("Blueprint'/Game/Origami/Blueprints/Actors/Bt_Act_Cocoon_S_2.Bt_Act_Cocoon_S_2'"));
	Objects.Add(TEXT("/Game/Origami/Blueprints/Actors/Bt_Act_Cocoon_S_2.Bt_Act_Cocoon_S_2"), Obj_Stc_Cocoon_S_2.Get());

	ConstructorHelpers::FObjectFinderOptional<class UBlueprint> Bt_Act_OrbGroup(TEXT("Blueprint'/Game/Origami/Blueprints/Actors/Bt_Act_OrbGroup.Bt_Act_OrbGroup'"));
	Objects.Add(TEXT("/Game/Origami/Blueprints/Actors/Bt_Act_OrbGroup.Bt_Act_OrbGroup"), Bt_Act_OrbGroup.Get());
}

AOrigamiGameMode::AOrigamiGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Origami/Blueprints/Bp_Player"));
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	this->ConstructorStatics.Initialize();
}
