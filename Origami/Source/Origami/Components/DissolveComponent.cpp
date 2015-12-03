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
	BaseMaterial = (UMaterial*)MaterialFinder.Object;
}


// Called when the game starts
void UDissolveComponent::BeginPlay()
{
	Super::BeginPlay();

	// Create the material instance
	MaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);

	// Find the actor's static mesh component(s)
	AActor* Actor = GetOwner();

	TArray<UMeshComponent*> Components;
	Actor->GetComponents<UMeshComponent>(Components);

	for (int32 i = 0; i<Components.Num(); i++)
	{
		// Assign the material instance
		UMeshComponent* MeshComponent = Components[i];
		MeshComponent->SetMaterial(0, MaterialInstance);
	}
}


// Called every frame
void UDissolveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bDissolve && DissolveValue < 1.0f)
		DissolveValue += 0.5f * DeltaTime;
	if (!bDissolve && DissolveValue > 0.0f)
		DissolveValue -= 0.5f * DeltaTime;

	if(MaterialInstance != NULL)
		MaterialInstance->SetScalarParameterValue(FName(TEXT("Dissolve Anmount")), DissolveValue);
}


bool UDissolveComponent::GetDissolve()
{
	return bDissolve;
}


void UDissolveComponent::SetDissolve(bool State)
{
	bDissolve = State;
}