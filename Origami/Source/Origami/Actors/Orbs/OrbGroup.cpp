#include "Origami.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Classes/Engine/StreamableManager.h"
#include "OrbGroup.h"

AOrbGroup::AOrbGroup(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->TravelledDistanceOnPath = 0.0f;
	this->Mode = EOrbMode::Swarm;
	this->OrbCount = 20;
	this->OrbColor = FColor::White;
	this->OrbSpawnBoxExtents = 40.0f;
	this->bIsGenerated = false;
	this->OrbMeshFileName = TEXT("StaticMesh'/Game/Origami/Meshes/OrbMesh.OrbMesh'");
	this->OrbPath = NULL;

	// Root Scene Component
	this->RootSceneComponent = CreateAbstractDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	this->RootSceneComponent->RelativeLocation = FVector::ZeroVector;
	this->RootComponent = this->RootSceneComponent;

	if (this->RootSceneComponent)
	{
		// Spline Component
		this->OrbPath = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
		if (this->OrbPath)
		{
			this->OrbPath->AttachTo(this->RootComponent);
			this->OrbPath->SetRelativeLocation(FVector::ZeroVector);
			//this->OrbPath->RegisterComponent();
		}
	}
}

void AOrbGroup::Tick(float deltaSeconds) 
{
	Super::Tick(deltaSeconds);

	//UKismetSystemLibrary::DrawDebugBox(GetWorld(), this->RootComponent->GetComponentLocation(), FVector(200), FLinearColor::Red, FRotator::ZeroRotator, 0.1f);
	//UKismetSystemLibrary::DrawDebugBox(GetWorld(), this->BoxSceneComponent->GetComponentLocation(), this->BoxSceneComponent->GetUnscaledBoxExtent(), FLinearColor::Blue, FRotator::ZeroRotator, 0.1f);

	if (!this->OrbPath)
		return;   

	float pathDistance = this->OrbPath->GetSplineLength();
	this->TravelledDistanceOnPath = FMath::FInterpConstantTo(this->TravelledDistanceOnPath, pathDistance, deltaSeconds, 100.0f);

	if (this->TravelledDistanceOnPath >= pathDistance)
		this->TravelledDistanceOnPath = 0.0f;

	FVector location = this->OrbPath->GetWorldLocationAtDistanceAlongSpline(TravelledDistanceOnPath);
	FRotator rotation = this->OrbPath->GetWorldRotationAtDistanceAlongSpline(TravelledDistanceOnPath);
	this->OrbsSceneComponent->SetWorldLocationAndRotation(location, rotation, true);
}

void AOrbGroup::BeginPlay()
{
	Super::BeginPlay();

	this->OrbPath->ResetRelativeTransform();

	// GenerateOrbs only at runtime 
	// if we do this within the constructor the editor becomes slow as fuck somehow?!
	this->GenerateOrbs();
}

void AOrbGroup::GenerateOrbs()
{
	// take care that we do not recreate the 
	// orbs!
	if (this->bIsGenerated)
		return;

	this->OrbsSceneComponent = ConstructObject<USceneComponent>(USceneComponent::StaticClass(), this, TEXT("Orbs"));
	if (!this->OrbsSceneComponent)
		return;

	UE_LOG(LogTemp, Warning, TEXT("Number of Spline points:  %i"), this->OrbPath->GetNumSplinePoints());

	FVector meshScale = FVector(0.2f, 0.2f, 0.2f);

	this->OrbsSceneComponent->SetRelativeLocation(FVector::ZeroVector);
	this->OrbsSceneComponent->AttachTo(this->RootComponent);
	this->OrbsSceneComponent->RegisterComponent();
	this->OrbsSceneComponent->ResetRelativeTransform();

	UStaticMesh* orbMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *this->OrbMeshFileName));
	if (!orbMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't load orb mesh with filename %s"), *this->OrbMeshFileName);
		return;
	}
	
	FBox spawnBox = FBox::BuildAABB(this->GetActorLocation(), FVector(this->OrbSpawnBoxExtents));

	// we know how many orbs we need to create upfront 
	// so we can reserve the memory on the initial creation!
	for (uint16 i = 0; i < this->OrbCount; i++)
	{
		FString staticMeshCompName = "Orb_" + FString::FromInt(i) + "_Mesh";
		FName fMeshName = FName(*staticMeshCompName);
		FVector location = FMath::RandPointInBox(spawnBox);
		//UE_LOG(LogTemp, Warning, TEXT("Location %f %f %f"), location.X, location.Y, location.Z);

		// now we can create a static mesh component!
		UStaticMeshComponent* meshComp = ConstructObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass(), this, fMeshName);
		if (meshComp) 
		{
			meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			meshComp->SetStaticMesh(orbMesh);
			meshComp->SetRelativeScale3D(meshScale);
			meshComp->SetRelativeLocation(location);
			meshComp->SetRelativeRotation(FRotator::ZeroRotator);
			meshComp->AttachTo(this->OrbsSceneComponent);
			meshComp->RegisterComponent();
		}
	}

	//FRotator rotation = this->OrbPath->GetWorldRotationAtDistanceAlongSpline(0);
	//rotation.Pitch = 90.0f;
	this->OrbsSceneComponent->SetWorldLocation(this->OrbPath->GetWorldLocationAtDistanceAlongSpline(0));
	this->bIsGenerated = true;
}