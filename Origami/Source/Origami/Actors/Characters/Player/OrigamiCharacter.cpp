// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Origami.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Origami/Actors/Entity.h"
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
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 500.0f; // The camera follows at this distance behind the character	
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

	// now reserve space for the maximal number of orbs groups the player can posses!
	this->Orbs.Reserve(3);
}

///////////////////////////////////////////////////////////////////////////
// UE4 Native Events
void AOrigamiCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(FindAimTimeHandle, this, &AOrigamiCharacter::FindAim, 0.2f, true);
	GetWorldTimerManager().SetTimer(IsWithinInteractionRangeHandle, this, &AOrigamiCharacter::CheckIfIsInInteractionRange, 0.3f, true);
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
					this->Target = entity;
					return;
				}
			}
		}
	}

	// reset the target once we are more than 10000.0f cms away!
	if (IsValid(this->Target))
	{
		FVector thisLocation = this->GetActorLocation();
		FVector targetLocation = this->Target->GetActorLocation();

		if (FVector::Dist(thisLocation, targetLocation) > 10000.0f)
			Target = NULL;
	}
}

void AOrigamiCharacter::CheckIfIsInInteractionRange()
{
	if (!IsValid(this->Target))
		return;

	FVector thisLocation = this->GetActorLocation();
	FVector targetLocation = this->Target->GetActorLocation();

	// is the player within 1.5 meters of the object?
	if (FVector::Dist(thisLocation, targetLocation) <= 1200.0f)
		this->bIsInInteractionRange = true;
	else
		this->bIsInInteractionRange = false;
}

void AOrigamiCharacter::AddOrbGroup(AOrbGroup* orbGroup)
{
	if (Orbs.Num() >= 3)
		return;

	Orbs.Add(orbGroup);
}

//////////////////////////////////////////////////////////////////////////
// Input
void AOrigamiCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	InputComponent->BindAction("Interact", IE_Pressed, this, &AOrigamiCharacter::Interact);

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

void AOrigamiCharacter::Interact()
{
	if (this->bIsInInteractionRange) 
	{
		this->Target->Interact(this);
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
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AOrigamiCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
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
