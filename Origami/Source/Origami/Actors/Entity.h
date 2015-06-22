// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Actors/Characters/Player/OrigamiCharacter.h"
#include "Entity.generated.h"

enum EMantleState {
	Mantled = 0, 
	Unmantled = 1
};

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
	void Dismantle();

	// Call this whenever this entity should be mantled by ink!
	void Mantle(); 

public:

	UFUNCTION(BlueprintPure, meta = (DisplayName = "New Entity from string", Keywords = "New Entity from string"), Category = Game)
	static AEntity* NewEntityFromString(AActor* actor, const FString Path, const FString Name);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "New Actor from string", Keywords = "New Actor from string"), Category = Game)
	static AActor* NewActorFromString(AActor* Actor, const FString Path, const FString Name);
	
protected:
	EMantleState State;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Entity, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* AimBox;

	void Interact(AOrigamiCharacter* character); 
};
