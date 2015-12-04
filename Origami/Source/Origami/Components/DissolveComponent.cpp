// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "DissolveComponent.h"


// Sets default values for this component's properties
UDissolveComponent::UDissolveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// Find the base material
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("Material'/Game/Origami/Materials/Mat_Dissolve.Mat_Dissolve'"));
	
	if(MaterialFinder.Succeeded())
		BaseMaterial = (UMaterial*)MaterialFinder.Object;
}


// Called when the game starts
void UDissolveComponent::BeginPlay()
{
	Super::BeginPlay();

	if(!BaseMaterial)
		return;

	// Create the material instance
	MaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);

	// Find the actor's static mesh component(s)
	AActor* Actor = GetOwner();

	TArray<UMeshComponent*> Components;
	Actor->GetComponents<UMeshComponent>(Components);

	// Assign the material instance to all of them
	for (int32 i = 0; i<Components.Num(); i++)
		Components[i]->SetMaterial(0, MaterialInstance);
}


// Called every frame
void UDissolveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(MaterialInstance)
	{
		// Dissolving
		if (bShouldDissolve && DissolveValue < 1.0f)
			DissolveValue += 1 / DissolveTime * DeltaTime;

		// Assembling
		if (!bShouldDissolve && DissolveValue > 0.0f)
			DissolveValue -= 1 / DissolveTime * DeltaTime;

		MaterialInstance->SetScalarParameterValue(FName(TEXT("Dissolve Anmount")), DissolveValue);
	}
}

void UDissolveComponent::Assemble()
{
	bShouldDissolve = false;
}

void UDissolveComponent::Dissolve()
{
	bShouldDissolve = true;
}

bool UDissolveComponent::IsFullyAssembled()
{
	return DissolveValue <= 0.0f;
}

bool UDissolveComponent::IsFullyDissolved()
{
	return DissolveValue >= 1.0f;
}