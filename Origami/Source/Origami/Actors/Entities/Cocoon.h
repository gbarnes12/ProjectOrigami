// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actors/Entity.h"
#include "Cocoon.generated.h"

/**
 * 
 */
UCLASS()
class ORIGAMI_API ACocoon : public AEntity
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = Cocoon)
	bool bSpawnOrbsAtStartup;

public:
	ACocoon();
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cocoon, meta = (AllowPrivateAccess = "true"))
	class USplineComponent* OrbPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cocoon, meta = (AllowPrivateAccess = "true"))
	class AOrbFlock* Orbs;

	class AActor* ActionButtonPrompt;
	
public:
	virtual void Interact(AOrigamiCharacter* player) override;
	virtual void EnterInteractionRange(AOrigamiCharacter* player, FVector collisionPoint) override;
	virtual void LeaveInteractionRange(AOrigamiCharacter* player) override;
	FORCEINLINE class USplineComponent* GetOrbPath() const { return this->OrbPath; }
};
