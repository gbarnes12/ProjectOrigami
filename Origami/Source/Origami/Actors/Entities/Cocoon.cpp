// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Actors/Orbs/OrbGroup.h"
#include "Cocoon.h"


ACocoon::ACocoon()
	: Super()
{
	USceneComponent* rootSceneComp = CreateAbstractDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	rootSceneComp->RelativeLocation = FVector::ZeroVector;
	this->RootComponent = rootSceneComp;

	this->OrbPath = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
	if (this->OrbPath)
	{
		this->OrbPath->AttachTo(this->RootComponent);
		this->OrbPath->SetClosedLoop(true);
		//this->OrbPath->SetRelativeLocation(FVector::ZeroVector);
	}
}

USplineComponent* ACocoon::GetOrbPath() const
{
	return this->OrbPath;
}

void ACocoon::BeginPlay() 
{
	Super::BeginPlay();

	if (!IsValid(this->OrbPath))
		return;

	AActor* rock = this->GetAttachParentActor();
	if (!IsValid(rock))
		return;
	//
	float radius = (rock->GetSimpleCollisionRadius() + this->GetSimpleCollisionRadius()) - 100.0f;
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


	// Now we need to spawn the OrbGroup :)
	AActor* orbGroup = AEntity::NewActorFromString(this, TEXT("/Game/Origami/Blueprints/Actors/"), TEXT("Bt_Act_OrbGroup.Bt_Act_OrbGroup"));
	if (!IsValid(orbGroup)) 
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't create a new instance of Bt_Act_OrbGroup blueprint!"));
		return;
	}

	this->Orbs = Cast<AOrbGroup>(orbGroup);
	this->Orbs->AttachSocket(this);
}

	
