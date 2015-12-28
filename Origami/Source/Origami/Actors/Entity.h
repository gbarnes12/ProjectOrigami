// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Entity.generated.h"

enum EMantleState {
	Mantled = 0, 
	Unmantled = 1
};

class AOrigamiCharacter;

UCLASS()
class ORIGAMI_API AEntity : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = Visual)
	bool bIsInteractable;

public:	
	// Sets default values for this actor's properties
	AEntity();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Call this whenever this actor becomes visible 
	virtual void Dismantle();

	// Call this whenever this entity should be mantled by ink!
	virtual void Mantle();

	// Can be called whenever the player interacts with this entity (via Key Press: Interact).
	virtual void Interact(AOrigamiCharacter* player);

	// Is called whenever the player is within interaction range and this entity is hit by his ray.
	virtual void EnterInteractionRange(AOrigamiCharacter* player, FVector collisionPoint);
	virtual void LeaveInteractionRange(AOrigamiCharacter* player);

public:

	UFUNCTION(BlueprintPure, meta = (DisplayName = "New Entity from string", Keywords = "New Entity from string"), Category = Game)
	static AEntity* NewEntityFromString(AActor* actor, const FString Path, const FString Name, bool AttachToParent);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "New Actor from string", Keywords = "New Actor from string"), Category = Game)
	static AActor* NewActorFromString(AActor* Actor, const FString Path, const FString Name, bool AttachToParent);
	
protected:
	EMantleState State;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Entity, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* AimBox;
};
