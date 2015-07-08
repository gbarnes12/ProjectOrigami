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
		this->Mesh->RegisterComponent();
		this->Mesh->SetRelativeLocation(FVector::ZeroVector);
		this->Mesh->SetRelativeRotation(FRotator::ZeroRotator);
	}

}

///////////////////////////////////////////////////////////////////////////
// UE4 Native Event

void AStaticEntity::BeginPlay()
{
	this->Mesh->SetVisibility(false);
}

///////////////////////////////////////////////////////////////////////////
// Entity Override Methods

void AStaticEntity::Dismantle()
{
	Super::Dismantle();
}

void AStaticEntity::Mantle()
{
	Super::Mantle();
}
