// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Origami.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Origami/Actors/Entity.h"
#include "Origami/Actors/Orbs/OrbGroup.h"
#include "OrigamiCharacter.h"

//////////////////////////////////////////////////////////////////////////
// AOrigamiCharacter

AOrigamiCharacter::AOrigamiCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{ 
	// Set target at null at first
	Target = NULL;

	// set that we aren't yet in interaction range!
	bIsInInteractionRange = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Set min and max zoom values
	MaxZoom = 700.0f;
	MinZoom = 480.0f;

	// Current color!
	CurrentColor = FColor::Green;

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.7f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 500.0f; // The camera follows at this distance behind the character	
	CameraBoom->SocketOffset = FVector(0.0, -500.f, 0.0);
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	// Create a path on which the orbs can travel!
	this->OrbPath = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
	if (this->OrbPath)
	{
		this->OrbPath->AttachTo(this->RootComponent);
		this->OrbPath->SetRelativeLocation(FVector::ZeroVector);
	}

	this->Orbs.Reserve(3);

	bIsCallingOrbs = false;
}

///////////////////////////////////////////////////////////////////////////
// UE4 Native Events
void AOrigamiCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(FindAimTimeHandle, this, &AOrigamiCharacter::FindAim, 0.2f, true);
	GetWorldTimerManager().SetTimer(IsWithinInteractionRangeHandle, this, &AOrigamiCharacter::CheckIfIsInInteractionRange, 0.3f, true);
}


void AOrigamiCharacter::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	// Is the character dead?
	if (IsDead())
	{
		FollowCamera->PostProcessSettings.VignetteIntensity *= 1.1f;
	}
	else
	{
		//if (CameraBoom->TargetArmLength != this->TargetZoom)
		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, this->TargetZoom, deltaSeconds, 1.3f);

		if (SideAmount == 0.0f && ForwardAmount == 0.0f)
			SetIsTargetMovingForOrbs(false);
		else
			SetIsTargetMovingForOrbs(true);
	}
}


//////////////////////////////////////////////////////////////////////////
/// Gameplay
void AOrigamiCharacter::FindAim()
{
	FVector cameraLocation = this->GetFollowCamera()->GetComponentLocation();
	FRotator cameraRotation = this->GetFollowCamera()->GetComponentRotation();

	this->GetActorEyesViewPoint(cameraLocation, cameraRotation);

	FVector start = cameraLocation;

	// calculate end within the next 1000 cms
	FVector end = cameraLocation + (cameraRotation.Vector() * 1000.0f);

	FCollisionQueryParams rvTraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	rvTraceParams.bTraceComplex = false; // we don't need complex collision for this trace since we only want to check if we 
	rvTraceParams.bTraceAsyncScene = true;
	rvTraceParams.bReturnPhysicalMaterial = false;

	//Re-initialize hit info
	FHitResult rvHit(ForceInit);

	/*GetWorld()->LineTraceSingleByChannel(
		rvHit, //result
		start, //start
		end, //end
		ECollisionChannel::ECC_Visibility, //collision channel
		rvTraceParams);

	if (rvHit.bBlockingHit) 
	{
		AActor* actor = rvHit.GetActor();
		if (IsValid(actor))
		{
			if (actor->IsA(AEntity::StaticClass())) 
			{
				//UKismetSystemLibrary::DrawDebugLine(GetWorld(), start, end, FLinearColor::Red, 1.0f, 2.0f);
				AEntity* entity = Cast<AEntity>(actor);
				if (IsValid(entity) && entity->bIsInteractable) 
				{
					this->Target = entity;
					return;
				}
			}
		}
	}
	*/
//	
	// reset the target once we are more than 10000.0f cms away!
	if (IsValid(this->Target))
	{
		this->Target->LeaveInteractionRange(this);
		Target = NULL;
	}

	// new end position!
	end += FVector(0.0f, 0.0f, 300.0f);

	// Second trace + 300 cms above in order to prevent always aiming to the top. 
	GetWorld()->LineTraceSingleByChannel(
		rvHit, //result
		start, //start
		end, //end
		ECollisionChannel::ECC_Visibility, //collision channel
		rvTraceParams);

	if (rvHit.bBlockingHit)
	{
		AActor* actor = rvHit.GetActor();
		if (IsValid(actor))
		{
			if (actor->IsA(AEntity::StaticClass()))
			{
				//UKismetSystemLibrary::DrawDebugLine(GetWorld(), start, end, FLinearColor::Red, 1.0f, 2.0f);
				AEntity* entity = Cast<AEntity>(actor);
				if (IsValid(entity) && entity->bIsInteractable)
				{
					//UKismetSystemLibrary::DrawDebugLine(GetWorld(), start, end, FLinearColor::Green, 0.2f);
					this->Target = entity;
					this->Target->EnterInteractionRange(this, rvHit.ImpactPoint);
					return;
				}
			}
		}
	}

//	UKismetSystemLibrary::DrawDebugLine(GetWorld(), start, end, FLinearColor::Red, 0.2f);

}

void AOrigamiCharacter::CheckIfIsInInteractionRange()
{
	if (!IsValid(this->Target))
		return;

	FVector thisLocation = this->GetActorLocation();
	FVector targetLocation = this->Target->GetActorLocation();

	// is the player within 1.5 meters of the object?
	if (FVector::Dist(thisLocation, targetLocation) <= 2000.0f)
		this->bIsInInteractionRange = true;
	else
		this->bIsInInteractionRange = false;
}

void AOrigamiCharacter::AddOrbGroup(AOrbGroup* orbGroup)
{
	//End calling orbs animation
	this->bIsCallingOrbs = false;
	this->EnableInput(Cast<APlayerController>(this->GetController()));

	if (this->Orbs.Num() == 3)
		return;	

	orbGroup->ChangeColor(this->CurrentColor);
	this->Orbs.Push(orbGroup);
}

void AOrigamiCharacter::ChangeColor(FColor color)
{
	this->CurrentColor = color;

	for (int i = 0; i < Orbs.Num(); i++)
	{
		AOrbGroup* orbGroup = this->Orbs[i];
		if (orbGroup)
		{
			orbGroup->ChangeColor(color);
		}
	}
}

void AOrigamiCharacter::SetIsTargetMovingForOrbs(bool value)
{
	for (int i = 0; i < this->Orbs.Num(); i++)
	{
		this->Orbs[i]->bIsTargetMoving = value;
	}
}

bool AOrigamiCharacter::IsDead()
{
	if (bIsDead)
		return true;

	// In case the character is below the death height, he dies
	if (this->GetActorLocation().Z < DeathHeight)
	{
		// When dead, the player is no longer able to interact
		this->DisableInput(GetWorld()->GetFirstPlayerController());

		// The camera should stay in position
		FollowCamera->DetachFromParent(true);

		// Execute the death method
		DeathEffects();

		return true;
	}

	return false;
}

void AOrigamiCharacter::DeathEffects()
{
	FollowCamera->PostProcessSettings.bOverride_VignetteIntensity = true;
}

//////////////////////////////////////////////////////////////////////////
// Input
void AOrigamiCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);

	/* Action bindings*/
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	InputComponent->BindAction("Interact", IE_Pressed, this, &AOrigamiCharacter::Interact);
	InputComponent->BindAction("Walk", IE_Pressed, this, &AOrigamiCharacter::Walk);
	InputComponent->BindAction("Walk", IE_Released, this, &AOrigamiCharacter::StopWalking);
	InputComponent->BindAction("Fire", IE_Pressed, this, &AOrigamiCharacter::Fire);
	InputComponent->BindAction("ChangeOrbColor_Red", IE_Pressed, this, &AOrigamiCharacter::ChangeOrbColor_Red);
	InputComponent->BindAction("ChangeOrbColor_Green", IE_Pressed, this, &AOrigamiCharacter::ChangeOrbColor_Green);
	InputComponent->BindAction("ChangeOrbColor_Blue", IE_Pressed, this, &AOrigamiCharacter::ChangeOrbColor_Blue);

	InputComponent->BindAxis("MoveForward", this, &AOrigamiCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AOrigamiCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AOrigamiCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AOrigamiCharacter::LookUpAtRate);

	// handle touch devices
	InputComponent->BindTouch(IE_Pressed, this, &AOrigamiCharacter::TouchStarted);
	InputComponent->BindTouch(IE_Released, this, &AOrigamiCharacter::TouchStopped);
}

void AOrigamiCharacter::ChangeOrbColor_Red()
{
	this->ChangeColor(FColor::Red);
}

void AOrigamiCharacter::ChangeOrbColor_Green()
{
	this->ChangeColor(FColor::Green);
}

void AOrigamiCharacter::ChangeOrbColor_Blue()
{
	this->ChangeColor(FColor::Blue);
}

void AOrigamiCharacter::Fire()
{
	if (this->Orbs.Num() == 0)
		return;

	AOrbGroup* group = this->Orbs.Pop(false);

	// check whether the group we returned is still valid
	if (IsValid(group))
	{
		FVector cameraLocation = this->GetFollowCamera()->GetComponentLocation();
		FRotator cameraRotation = this->GetFollowCamera()->GetComponentRotation();

		this->GetActorEyesViewPoint(cameraLocation, cameraRotation);

		FVector start = cameraLocation;
		FVector end = cameraLocation + (cameraRotation.Vector() * 2000.0f) + FVector(0.0f, 0.0f, 300.0f);

		group->StartMoveToTarget(NULL, end);
	}
}

void AOrigamiCharacter::Walk()
{
	this->GetCharacterMovement()->MaxWalkSpeed = 300;
}

void AOrigamiCharacter::StopWalking()
{
	this->GetCharacterMovement()->MaxWalkSpeed = 600;
}

void AOrigamiCharacter::Interact()
{
	if (this->bIsInInteractionRange) 
	{
		if (this->Target) 
		{
			this->Target->Interact(this);
		}
	}
}

void AOrigamiCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void AOrigamiCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}

void AOrigamiCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AOrigamiCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AOrigamiCharacter::MoveForward(float Value)
{
	ForwardAmount = Value;

	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	
		TargetZoom = this->MaxZoom;
		return;
	}

	TargetZoom = this->MinZoom;
	
}

void AOrigamiCharacter::MoveRight(float Value)
{
	SideAmount = Value;

	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
		return;
	}
}



///////////////////////////////////////////////////////////////////////////
// Static Blueprint Methods
FString AOrigamiCharacter::GetCurrentLevel(AActor * sourceActor)
{
	if (sourceActor == NULL)
	{
		return FString(TEXT("Must have a sourceActor (was NULL)"));
	}

	return sourceActor->GetWorld()->GetMapName();
}
