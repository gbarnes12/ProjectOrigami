// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "OrigamiCharacter.generated.h"

UCLASS(config=Game)
class AOrigamiCharacter : public ACharacter
{
	GENERATED_BODY()

	/* Handle to the timer that will adjust the speed periodically */
	struct FTimerHandle FindAimTimeHandle;

	/* Is the player within the interaction range? */
	struct FTimerHandle IsWithinInteractionRangeHandle;

	/* The current Aim we are looking at! */
	class AEntity* Target;

	/*Specifies if the player is within the necessary interaction range of an entity.*/
	bool bIsInInteractionRange;

	/*The Orbs the player currently posseses*/
	TArray<class AOrbGroup*> Orbs;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	AOrigamiCharacter(const FObjectInitializer& ObjectInitializer);

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditAnywhere, Category = OrigamiCharacter)
	float IneractionRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = OrigamiCharacter, meta = (AllowPrivateAccess = "true"))
	class USplineComponent* OrbPath;

public:
	/*  */
	UFUNCTION(BlueprintCallable, Category = TDLHelpers)
	static FString GetCurrentLevel(AActor * sourceActor);

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	/** Called via input to interact with the environment. */
	void Interact();

	/*Called via input in order to let the character walk instead of run!*/
	void Walk();

	/*Called via input in order to let the character run again.*/
	void StopWalking();

	/*Called via input in order to "shoot" orbs.*/
	void Fire();

	/*Called via input in order to switch between orb colors*/
	void ChangeOrbColor();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/* Begin play */
	virtual void BeginPlay() override;


	/*  */
	void AddOrbGroup(AOrbGroup* orbGroup);

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
	/* Called every 0.2f seconds in order to find an active aim the player is looking at. */
	void FindAim();
	
	/*Will check whether the player is within the interaction range of a target or not!*/
	void CheckIfIsInInteractionRange();
};

