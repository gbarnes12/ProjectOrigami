// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "DissolveComponent.h"


// Sets default values for this component's properties
UDissolveComponent::UDissolveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = false;
	PrimaryComponentTick.bCanEverTick = true;

	//// Get the material instance
	//static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Origami/Materials/Mat_Dissolve.Mat_Dissolve'"));

	//if (Material.Object != NULL)
	//{
	//	UMaterial* MaterialObject = (UMaterial*)Material.Object;
	//	TheMaterial = UMaterialInstanceDynamic::Create(MaterialObject, this);
	//}

	//// Get the parent actor
	//AActor* Actor = GetOwner();
	//
	//// Find the actor's static mesh component(s)
	//TArray<UStaticMeshComponent*> Components;
	//Actor->GetComponents<UStaticMeshComponent>(Components);
	//
	//for(int32 i = 0; i<Components.Num(); i++)
	//{
	//	UStaticMeshComponent* StaticMeshComponent = Components[i];

	//	// Set the material to our material instance
	//	StaticMeshComponent->SetMaterial(0, TheMaterial);
	//}
}


// Called when the game starts
void UDissolveComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UDissolveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	/*Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(tempDissolveValue < 1.0f)
		tempDissolveValue += 0.05f * DeltaTime;

	TheMaterial->SetScalarParameterValue(FName(TEXT("Dissolve Anmount")), tempDissolveValue);*/
}


void UDissolveComponent::SetDissolve(bool State)
{
	/*bDissolve = State;*/
}