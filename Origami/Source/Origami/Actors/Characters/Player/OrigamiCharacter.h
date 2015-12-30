// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Origami/Actors/Entities/Cocoon.h"
#include "OrigamiCharacter.generated.h"

UCLASS(config=Game)
class AOrigamiCharacter : public ACharacter
{
	GENERATED_BODY()

/////////////////////////////////////////////////////////////////////////////////////
//                           This has to be removed                                //
public:
	UPROPERTY(EditAnywhere, Category = Teleporter)
		float Teleporter = 0;
//                           This has to be removed                                //
/////////////////////////////////////////////////////////////////////////////////////


	/* The character dies, when he falls of the edge */
	bool bIsDead = false;

	/* Handle to the timer that will adjust the speed periodically */
	struct FTimerHandle FindAimTimeHandle;

	/* Is the player within the interaction range? */
	struct FTimerHandle IsWithinInteractionRangeHandle;

	/* The current Aim we are looking at! */
	class ACocoon* Target;

	/*Specifies if the player is within the necessary interaction range of an entity.*/
	bool bIsInInteractionRange;

	/*The target zoom which we currently interpolate too*/
	float TargetZoom;

	/*The Current Color which the orbs will change to.*/
	FColor CurrentColor;

	/*The Orbs the player currently posseses*/
	TArray<class AOrbFlock*> Orbs;

	/*Temp values*/
	float ForwardAmount;
	float SideAmount;

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

	/** The min zoom the camera is at.  */
	UPROPERTY(EditAnywhere, Category = OrigamiCharacter)
	float MinZoom;

	/** the max zoom the camera can interpolate to. */
	UPROPERTY(EditAnywhere, Category = OrigamiCharacter)
	float MaxZoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = OrigamiCharacter, meta = (AllowPrivateAccess = "true"))
	class USplineComponent* OrbPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = OrigamiCharacter)
	bool bIsCallingOrbs;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = OrigamiCharacter)
	bool bIsSendingOrbs;

	UPROPERTY(EditAnywhere, Category = OrigamiCharacter)
	float DeathHeight = -3000;

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
	* Called via input to turn at standard rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void Turn(float Rate);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at standard rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUp(float Rate);

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
	void ChangeOrbColor_Red();
	void ChangeOrbColor_Green();
	void ChangeOrbColor_Blue();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/* Begin play */
	virtual void BeginPlay() override;

	/* Tick */
	virtual void Tick(float deltaSeconds) override;

	/*  */
	UFUNCTION(BlueprintCallable, Category = "Flock")
	void AddOrbFlock(AOrbFlock* orbGroup);

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:

	/* Changes the color of the orbs.*/
	void ChangeColor(FColor color);

	/* Called every 0.2f seconds in order to find an active aim the player is looking at. */
	void FindAim();
	
	/*Will check whether the player is within the interaction range of a target or not!*/
	void CheckIfIsInInteractionRange();

	/* Tells the orbs to stop moving around the path or to start again! */
	void SetIsTargetMovingForOrbs(bool value);

	/* In case, the character is below the death height, he dies */
	bool IsDead();

	/* Creates a visual feedback, which is triggered, when the character dies */
	void DeathEffects();
};

