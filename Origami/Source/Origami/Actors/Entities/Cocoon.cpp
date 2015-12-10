// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Actors/Orbs/OrbGroup.h"
#include "Actors/Orbs/OrbFlock.h"
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
}

///////////////////////////////////////////////////////////////////////////
// UE4 Native Event

void ACocoon::BeginPlay() 
{
	Super::BeginPlay();

	if (bSpawnOrbsAtStartup)
	{

		FActorSpawnParameters params;
		params.bNoCollisionFail = true;

		AOrbFlock* flock = GetWorld()->SpawnActor<AOrbFlock>(AOrbFlock::StaticClass(), this->GetActorLocation(), this->GetActorRotation(), params);

		// Now we need to spawn the OrbGroup :)
		if (!IsValid(flock))
		{
			UE_LOG(LogTemp, Error, TEXT("Couldn't create a new instance of AOrbFlock!"));
			return;
		}
		
		this->Orbs = flock;
		this->Orbs->InitializeAiController();
		this->Orbs->AttachToEntity(this);
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
	if (!IsValid(this->ActionButtonPrompt))
		return;

	this->ActionButtonPrompt->SetActorHiddenInGame(true);
}


void ACocoon::Interact(AOrigamiCharacter* player)
{
	if (!IsValid(this->Orbs))
		return;

	UE_LOG(LogTemp, Log, TEXT("Orbs: %s"), *this->Orbs->GetName());

	this->Orbs->DetachFromEntity();
	this->Orbs->AttachToEntity(player);
	this->Orbs->ResetTargetTo(player->GetActorLocation());
	//this->Orbs->StartMoveToTarget(player, player->OrbPath->GetWorldLocationAtSplinePoint(0), true); //TODO: Should somehow catch if player already has max number of orbs before blindly send orbs on their way?
	player->DisableInput(Cast<APlayerController>(player->GetController()));
	
	//Play calling orb animation
	player->bIsCallingOrbs = true;

	this->Orbs = NULL;
}