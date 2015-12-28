// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "DissolveComponent.h"


// Sets default values for this component's properties
UDissolveComponent::UDissolveComponent()
	: Super()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// Find the base material
}


// Called when the game starts
void UDissolveComponent::BeginPlay()
{
	Super::BeginPlay();

	// Find the actor's static mesh component(s)
	AActor* Actor = GetOwner();

	UActorComponent* component = Actor->GetComponentByClass(UMeshComponent::StaticClass());
	if (!component)
		return;

	meshComponent = Cast<UMeshComponent>(component);
	if (!meshComponent)
		return;

	MaterialInstance = UMaterialInstanceDynamic::Create(meshComponent->GetMaterial(0)->GetMaterial(), this);
	meshComponent->SetMaterial(0, MaterialInstance);
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

	if (!meshComponent)
		return;

	meshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void UDissolveComponent::Dissolve()
{
	bShouldDissolve = true;

	if (!meshComponent)
		return;

	meshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

bool UDissolveComponent::IsFullyAssembled()
{
	return DissolveValue <= 0.0f;
}

bool UDissolveComponent::IsFullyDissolved()
{
	return DissolveValue >= 1.0f;
}