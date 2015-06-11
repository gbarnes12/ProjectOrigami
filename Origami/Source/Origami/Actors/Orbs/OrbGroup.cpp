#include "Origami.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "OrbGroup.h"

AOrbGroup::AOrbGroup(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->OrbSpeedLevel.Reserve(3);
	this->OrbSpeedLevel.Add(100.0f);
	this->OrbSpeedLevel.Add(160.0f);
	this->OrbSpeedLevel.Add(240.0f);

	this->TravelledDistanceOnPath = 0.0f;
	this->MovementSpeed = OrbSpeedLevel[0];
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
		}
	}
}

void AOrbGroup::BeginPlay()
{
	Super::BeginPlay();

	this->OrbPath->ResetRelativeTransform();

	// GenerateOrbs only at runtime 
	// if we do this within the constructor the editor becomes slow as fuck somehow?!
	this->GenerateOrbs();

	/*Start the timer that will adjust the speed over time*/
	GetWorldTimerManager().ClearTimer(AdjustSpeedTimerHandle);
	GetWorldTimerManager().SetTimer(AdjustSpeedTimerHandle, this, &AOrbGroup::AdjustSpeed, 5.0f, true);
	UE_LOG(LogTemp, Warning, TEXT("Created timer"));
}


void AOrbGroup::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	//UKismetSystemLibrary::DrawDebugBox(GetWorld(), this->RootComponent->GetComponentLocation(), FVector(200), FLinearColor::Red, FRotator::ZeroRotator, 0.1f);
	//UKismetSystemLibrary::DrawDebugBox(GetWorld(), this->BoxSceneComponent->GetComponentLocation(), this->BoxSceneComponent->GetUnscaledBoxExtent(), FLinearColor::Blue, FRotator::ZeroRotator, 0.1f);

	if (!this->OrbsSceneComponent->IsValidLowLevelFast())
		return;

	if (!this->OrbPath)
		return;

	float pathDistance = this->OrbPath->GetSplineLength();
	this->TravelledDistanceOnPath = FMath::FInterpConstantTo(this->TravelledDistanceOnPath, pathDistance, deltaSeconds, this->MovementSpeed);

	if (this->TravelledDistanceOnPath >= pathDistance)
		this->TravelledDistanceOnPath = 0.0f;

	const FVector location = this->OrbPath->GetWorldLocationAtDistanceAlongSpline(TravelledDistanceOnPath);
	const FRotator rotation = this->OrbPath->GetWorldRotationAtDistanceAlongSpline(TravelledDistanceOnPath);

	this->OrbsSceneComponent->SetWorldLocationAndRotation(location, rotation, true);
}

void AOrbGroup::AdjustSpeed() 
{

	if (this->MovementSpeed == OrbSpeedLevel[0])
		this->MovementSpeed = OrbSpeedLevel[2];
	else if (this->MovementSpeed == OrbSpeedLevel[2])
		this->MovementSpeed = OrbSpeedLevel[1];
	else if (this->MovementSpeed == OrbSpeedLevel[1])
		this->MovementSpeed = OrbSpeedLevel[0];

	UE_LOG(LogTemp, Log, TEXT("Movement Speed: %f"), this->MovementSpeed);
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

	const FVector meshScale = FVector(0.2f, 0.2f, 0.2f);

	this->OrbsSceneComponent->SetRelativeLocation(FVector::ZeroVector);
	this->OrbsSceneComponent->AttachTo(this->RootComponent);
	this->OrbsSceneComponent->RegisterComponent();
	this->OrbsSceneComponent->ResetRelativeTransform();

	// load static mesh
	this->OrbMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *this->OrbMeshFileName));
	if (!this->OrbMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't load orb mesh with filename %s"), *this->OrbMeshFileName);
		return;
	}

	this->OrbMaterialInstance = UMaterialInstanceDynamic::Create(this->OrbMesh->GetMaterial(0), this);
	if (!this->OrbMaterialInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't create dynamic material instance from mesh with filename %s"), *this->OrbMeshFileName);
		return;
	}


	// create the area in which we want to spawn the orbs 
	const FBox spawnBox = FBox::BuildAABB(this->GetActorLocation(), FVector(this->OrbSpawnBoxExtents));

	// we know how many orbs we need to create upfront 
	// so we can reserve the memory on the initial creation!
	for (uint16 i = 0; i < this->OrbCount; i++)
	{
		const FString staticMeshCompName = "Orb_" + FString::FromInt(i) + "_Mesh";
		const FName fMeshName = FName(*staticMeshCompName);
		const FVector location = FMath::RandPointInBox(spawnBox);
		//UE_LOG(LogTemp, Warning, TEXT("Location %f %f %f"), location.X, location.Y, location.Z);

		// now we can create a static mesh component!
		UStaticMeshComponent* meshComp = ConstructObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass(), this, fMeshName);
		if (meshComp) 
		{
			meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			meshComp->SetStaticMesh(this->OrbMesh);
			meshComp->SetRelativeScale3D(meshScale);
			meshComp->SetRelativeLocation(location);
			meshComp->SetRelativeRotation(FRotator::ZeroRotator);
			meshComp->AttachTo(this->OrbsSceneComponent);
			meshComp->RegisterComponent();
		}
	}

	this->OrbsSceneComponent->SetWorldLocation(this->OrbPath->GetWorldLocationAtDistanceAlongSpline(0));
	this->bIsGenerated = true;
}