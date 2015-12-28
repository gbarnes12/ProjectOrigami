// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "OrbInteractable.h"


// Sets default values
AOrbInteractable::AOrbInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bIsUsable = true;

	// the attachment point for orbs!
	
	USceneComponent* sceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	if (sceneComponent)
	{
		sceneComponent->SetRelativeLocation(FVector::ZeroVector);
		this->RootComponent = sceneComponent;
	}
}

void AOrbInteractable::BeginPlay() {

	Super::BeginPlay();

	AttachmentPoint = this->GetActorLocation() + FVector(0.0f, 0.0f, 1.0f) * 200.5f;

}

// Orbs might interact with this actor
void AOrbInteractable::TriggerOrbInteraction(AOrbFlock* IncomingOrbs)
{
	bIsUsable = false;
}

// check whether or not this actor is usable by orbs!
bool AOrbInteractable::GetIsUsable()
{
	return bIsUsable;
}