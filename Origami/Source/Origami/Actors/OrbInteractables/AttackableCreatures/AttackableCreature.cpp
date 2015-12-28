// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "AttackableCreature.h"
#include "Components/OrbTriggerComponents/Red/OrbTriggerRedComponent.h"


// Sets default values
AAttackableCreature::AAttackableCreature()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Overwrite the default trigger component to enable the color check
	TriggerComponent = CreateDefaultSubobject<UOrbTriggerComponent>(TEXT("OrbTriggerRedComponent"));
}

// Orbs might interact with this actor
void AAttackableCreature::TriggerOrbInteraction(AOrbFlock* IncomingOrbs)
{
	Super::TriggerOrbInteraction(IncomingOrbs);

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
	//                                   //
	//            Ink Shader             //
	//                                   //
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
}