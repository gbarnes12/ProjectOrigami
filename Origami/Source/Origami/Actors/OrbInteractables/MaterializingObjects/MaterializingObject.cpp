// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "Actors/Orbs/OrbFlock.h"
#include "MaterializingObject.h"
#include "Components/OrbTriggerComponents/Green/OrbTriggerGreenComponent.h"


// Sets default values
AMaterializingObject::AMaterializingObject()
	: Super()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set the dissolving component to be able to fade the object in and out
	DissolveComponent = CreateDefaultSubobject<UDissolveComponent>(TEXT("DissolveComponent"));

	// Overwrite the default trigger component to enable the color check
	TriggerComponent = CreateDefaultSubobject<UOrbTriggerGreenComponent>(TEXT("OrbTriggerGreenComponent"));
}

// Called when the game starts
void AMaterializingObject::BeginPlay()
{
	Super::BeginPlay();

	// The timer has to be prepared for the first usage
	FadeOutTimer = TimeToFadeOut;
}

// Called every frame
void AMaterializingObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DissolveComponent->IsFullyAssembled())
	{
		// Maybe the time has come to start fading out again?
		if (DeltaTime >= FadeOutTimer)
		{
			DissolveComponent->Dissolve();
			if (Flock) 
				Flock->DetachFromEntity(EOrbMode::FindCocoon);

			Flock = NULL;
			FadeOutTimer = TimeToFadeOut;
		}

		// No? We need to wait and subtract the elapsed time
		else
		{
			FadeOutTimer -= DeltaTime;
		}
	}
}

// Orbs might interact with this actor
void AMaterializingObject::TriggerOrbInteraction(AOrbFlock* IncomingOrbs)
{
	Super::TriggerOrbInteraction(IncomingOrbs);

	Flock = IncomingOrbs;

	// On interaction the dissolving has to be deactivated to fade the object in
	DissolveComponent->Assemble();
}