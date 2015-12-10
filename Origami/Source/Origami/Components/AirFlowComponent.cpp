// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "AirFlowComponent.h"


// Sets default values for this component's properties
UAirFlowComponent::UAirFlowComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UAirFlowComponent::BeginPlay()
{
	Super::BeginPlay();

	static ACharacter* character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!IsValid(character))
		return;

	// Get the players movement component
	UPawnMovementComponent* pawnMovement = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetMovementComponent();
	if (!IsValid(pawnMovement))
		return;

	// To use the full functionality, it has to be converted to a character movement component
	if (pawnMovement != nullptr && pawnMovement->IsA<UCharacterMovementComponent>())
		CharacterMovement = Cast<UCharacterMovementComponent>(pawnMovement);
	else
		PrimaryComponentTick.bCanEverTick = false;
}


// Called every frame
void UAirFlowComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (!IsValid(CharacterMovement))
		return;

	// Get the vector from the flower to our character
	FVector difference = CharacterMovement->GetActorLocation() - GetOwner()->GetActorLocation();

	// Transform the vector into the local coordinatesystem of the flower
	FVector differenceRelative = FVector(
		difference.X * (GetOwner()->GetActorForwardVector().X + GetOwner()->GetActorForwardVector().Y + GetOwner()->GetActorForwardVector().Z),
		difference.Y * (GetOwner()->GetActorRightVector().X + GetOwner()->GetActorRightVector().Y + GetOwner()->GetActorRightVector().Z),
		difference.Z * (GetOwner()->GetActorUpVector().X + GetOwner()->GetActorUpVector().Y + GetOwner()->GetActorUpVector().Z)
	);

	// Calculate the angle between the flow-direction and the character-vector
	float angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FVector::UpVector, differenceRelative) / FVector::Dist(FVector(), differenceRelative)));

	// Only if the character is in the forcecone, the force has to be calculated
	if (differenceRelative.Z < MaxDistance && angle < MaxAngle)
	{
		// Calculate the basic intensity based on the distance from the flower
		float distanceIntensity = 1.0f - FMath::Pow(differenceRelative.Z / MaxDistance, 2);

		// Get the intensity based on the angle-variation
		float angleIntensity = 1.0f - FMath::Pow(angle / MaxAngle, 2);

		// Apply the specific force to the characters mesh
		CharacterMovement->AddForce(GetOwner()->GetActorUpVector() * FlowStrength * distanceIntensity * angleIntensity);
	}
}

