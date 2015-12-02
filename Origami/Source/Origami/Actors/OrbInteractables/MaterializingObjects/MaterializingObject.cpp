// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "MaterializingObject.h"
#include "Components/OrbTriggerComponents/Green/OrbTriggerGreenComponent.h"


// Sets default values
AMaterializingObject::AMaterializingObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Overwrite the default trigger component to enable the color check
	TriggerComponent->DestroyComponent();
	TriggerComponent = CreateDefaultSubobject<UOrbTriggerComponent>(TEXT("OrbTriggerGreenComponent"));
}

// Called every frame
void AMaterializingObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
	//                                   //
	//         Dissolve Shader           //
	//                                   //
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
}

// Orbs might interact with this actor
void AMaterializingObject::TriggerOrbInteraction(AOrbGroup* IncomingOrbs)
{
	Super::TriggerOrbInteraction(IncomingOrbs);

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
	//                                   //
	//         Dissolve Shader           //
	//                                   //
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
}