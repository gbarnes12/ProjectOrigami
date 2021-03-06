// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Actors/Orbs/OrbGroup.h"
#include "Cocoon.h"


///////////////////////////////////////////////////////////////////////////
// ACocoon
ACocoon::ACocoon()
	: Super()
{
	// Set this to true since the player needs to be able to interact with us
	this->bIsInteractable = true;
	this->ActionButtonPrompt = NULL;
	this->bSpawnOrbsAtStartup = true;
	this->Orbs = NULL;

	// set box extents 
	if (IsValid(this->AimBox))
	{
		FVector boxExtents = FVector(this->GetSimpleCollisionRadius());// * 2.2f;
		//boxExtents.Z = boxExtents.Z * 1.5f;
		this->AimBox->SetBoxExtent(boxExtents);
	}

	//  
	this->OrbPath = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
	if (this->OrbPath)
	{
		this->OrbPath->AttachTo(this->RootComponent);
		this->OrbPath->SetClosedLoop(true);
		//this->OrbPath->SetRelativeLocation(FVector::ZeroVector);
	}
}

///////////////////////////////////////////////////////////////////////////
// UE4 Native Event

void ACocoon::BeginPlay() 
{
	Super::BeginPlay();

	if (!IsValid(this->OrbPath))
		return;

	float radius = (this->GetSimpleCollisionRadius()) * 1.5f;
	this->OrbPath->ClearSplinePoints();

	// TODO: REFACTOR THIS SO IT MIGHT BE USED WITHIN A BLUEPRINT 
	// AND AT MULTIPLE CODE PLACES!
	FVector up = this->GetActorUpVector();
	FVector radiusVector = FVector(radius, 0.0f, 0.0f);
	FVector worldPosition = this->GetActorLocation();
	for (int i = 0; i <= 270; i = i + 45)
	{
		FVector pointOnRadius = UKismetMathLibrary::RotateAngleAxis(radiusVector, i, up);
		FVector randPosition = FVector(0.0f, 0.0f, FMath::RandRange(-10.0f, 80.0f));
		FVector position = worldPosition + pointOnRadius + randPosition;

		this->OrbPath->AddSplineWorldPoint(position);
	}

	this->OrbPath->ResetRelativeTransform();
	this->OrbPath->SetRelativeLocation(FVector::ZeroVector);

	if (bSpawnOrbsAtStartup)
	{
		// Now we need to spawn the OrbGroup :)
		AActor* orbGroup = AEntity::NewActorFromString(this, TEXT("/Game/Origami/Objects/Orb/"), TEXT("Bt_Act_OrbGroup.Bt_Act_OrbGroup"), false);
		if (!IsValid(orbGroup))
		{
			UE_LOG(LogTemp, Error, TEXT("Couldn't create a new instance of Bt_Act_OrbGroup blueprint!"));
			return;
		}

		this->Orbs = Cast<AOrbGroup>(orbGroup);
		this->Orbs->AttachSocket(this);
	}

	this->ActionButtonPrompt = AEntity::NewActorFromString(this, TEXT("/Game/Origami/Objects/UI/Hud/ActionPrompt/"), TEXT("Bt_ActionPrompt.Bt_ActionPrompt"), false);
	if (!IsValid(this->ActionButtonPrompt))
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't create a new instance of Bt_ActionPrompt blueprint!"));
		return;
	}

	this->ActionButtonPrompt->AttachRootComponentToActor(this);
	this->ActionButtonPrompt->SetActorHiddenInGame(true);
}

///////////////////////////////////////////////////////////////////////////
// Gameplay
void ACocoon::EnterInteractionRange(AOrigamiCharacter* player, FVector collisionPoint)
{
	if (!IsValid(this->ActionButtonPrompt) || !IsValid(player) || !this->Orbs)
		return;

	FVector forward = player->GetActorLocation() - collisionPoint;
	this->ActionButtonPrompt->SetActorLocation(collisionPoint + forward * 0.05f);
	this->ActionButtonPrompt->SetActorHiddenInGame(false);
}

void ACocoon::LeaveInteractionRange(AOrigamiCharacter* player)
{
	this->ActionButtonPrompt->SetActorHiddenInGame(true);
}


void ACocoon::Interact(AOrigamiCharacter* player)
{
	if (!IsValid(this->Orbs))
		return;

	//this->Orbs->AttachSocket(player);
	this->Orbs->StartMoveToTarget(player, player->OrbPath->GetWorldLocationAtSplinePoint(0), true); //TODO: Should somehow catch if player already has max number of orbs before blindly send orbs on their way?
	player->DisableInput(Cast<APlayerController>(player->GetController()));
	
	//Play calling orb animation
	player->bIsCallingOrbs = true;

	this->Orbs = NULL;
}