#include "Origami.h"
#include "StaticEntity.h"

///////////////////////////////////////////////////////////////////////////
// AStaticEntity
AStaticEntity::AStaticEntity()
	: Super()
{
	this->Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	if (this->Mesh)
	{
		this->Mesh->AttachTo(this->RootComponent);
		this->Mesh->SetRelativeLocation(FVector::ZeroVector);
		this->Mesh->SetRelativeRotation(FRotator::ZeroRotator);
	//
	}

}

///////////////////////////////////////////////////////////////////////////
// UE4 Native Event

void AStaticEntity::BeginPlay()
{
	Mantle();
	this->Mesh->RegisterComponent();
}

///////////////////////////////////////////////////////////////////////////
// Entity Override Methods

void AStaticEntity::Dismantle()
{
	this->Mesh->SetVisibility(true);
	this->SetActorEnableCollision(true);
	Super::Dismantle();
}

void AStaticEntity::Mantle()
{
	this->Mesh->SetVisibility(false);
	this->SetActorEnableCollision(false);
	Super::Mantle();
}
