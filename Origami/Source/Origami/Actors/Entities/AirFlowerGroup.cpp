// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "AirFlowerGroup.h"

///////////////////////////////////////////////////////////////////////////
// AirFlowerGroup
AAirFlowerGroup::AAirFlowerGroup()
	: Super()
{
	// Set this to true since the player needs to be able to interact with us
	this->bIsInteractable = true;
}

///////////////////////////////////////////////////////////////////////////
// UE4 Native Event
void AAirFlowerGroup::BeginPlay()
{
	Super::BeginPlay();

	// The aim box has to include all the airflowers
	float minX = TNumericLimits<float>::Max();
	float maxX = TNumericLimits<float>::Lowest();
	float minY = TNumericLimits<float>::Max();
	float maxY = TNumericLimits<float>::Lowest();
	float minZ = TNumericLimits<float>::Max();
	float maxZ = TNumericLimits<float>::Lowest();

	for (TArray<AAirFlower*>::TConstIterator PkgIter(AirFlowers); PkgIter; ++PkgIter)
	{
		// Access the element at the current position of the iterator with the * operator
		AAirFlower* airFlower = *PkgIter;
		FVector flowerLocation = airFlower->GetActorLocation();

		// If the location is out of the bounds, extend them
		if (flowerLocation.X < minX)
			minX = flowerLocation.X;
		if (flowerLocation.X > maxX)
			maxX = flowerLocation.X;
		if (flowerLocation.Y < minY)
			minY = flowerLocation.Y;
		if (flowerLocation.Y > maxY)
			maxY = flowerLocation.Y;
		if (flowerLocation.Z < minZ)
			minZ = flowerLocation.Z;
		if (flowerLocation.Z > maxZ)
			maxZ = flowerLocation.Z;
	}

	// Calculation of the range in every axis (Minimum range is 250)
	float rangeX = 250;
	float rangeY = 250;
	float rangeZ = 250;

	// X-Axis
	if (maxX > minX)
		rangeX += maxX - minX;

	// Y-Axis
	if (maxY > minY)
		rangeY += maxY - minY;

	// X-Axis
	if (maxZ > minZ)
		rangeZ += maxZ - minZ;

	// Set the volume of the AimBox
	this->AimBox->SetBoxExtent(FVector(rangeX, rangeY, rangeZ));

	// Set the location of the AimBox to the middle of the FlowerGroup
	this->AimBox->SetWorldLocation(FVector((minX + maxX) / 2, (minY + maxY) / 2, (minZ + maxZ) / 2));
}

///////////////////////////////////////////////////////////////////////////
// Gameplay
void AAirFlowerGroup::EnterInteractionRange(AOrigamiCharacter* player, FVector collisionPoint)
{
	// Call the method for all the GroupMembers
	for (TArray<AAirFlower*>::TConstIterator PkgIter(AirFlowers); PkgIter; ++PkgIter)
	{
		// Access the element at the current position of the iterator with the * operator
		AAirFlower* airFlower = *PkgIter;
		airFlower->EnterInteractionRange(player, collisionPoint);
	}
}

void AAirFlowerGroup::LeaveInteractionRange(AOrigamiCharacter* player)
{
	// Call the method for all the GroupMembers
	for (TArray<AAirFlower*>::TConstIterator PkgIter(AirFlowers); PkgIter; ++PkgIter)
	{
		// Access the element at the current position of the iterator with the * operator
		AAirFlower* airFlower = *PkgIter;
		airFlower->LeaveInteractionRange(player);
	}
}


void AAirFlowerGroup::Interact(AOrigamiCharacter* player)
{
	// Call the method for all the GroupMembers
	for (TArray<AAirFlower*>::TConstIterator PkgIter(AirFlowers); PkgIter; ++PkgIter)
	{
		// Access the element at the current position of the iterator with the * operator
		AAirFlower* airFlower = *PkgIter;
		airFlower->Interact(player);
	}
}