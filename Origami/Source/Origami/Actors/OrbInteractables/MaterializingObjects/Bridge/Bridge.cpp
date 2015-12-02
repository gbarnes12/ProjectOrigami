// Fill out your copyright notice in the Description page of Project Settings.

#include "Origami.h"
#include "Bridge.h"


// Sets default values
ABridge::ABridge()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a new SkeletalMeshComponent
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	// Find this mesh in the source folders
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Origami/Meshes/Level1_2/Obj_Stc_Lvl1_2_bridge.Obj_Stc_Lvl1_2_bridge"));

	// If it is found, set it as the mesh
	if (MeshFinder.Succeeded())
	{
		Mesh->SetStaticMesh(MeshFinder.Object);
	}
}