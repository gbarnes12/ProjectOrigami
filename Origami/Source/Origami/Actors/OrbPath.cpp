#include "Origami.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Actors/Entity.h"
#include "Actors/Orbs/OrbGroup.h"
#include "OrbPath.h"

///////////////////////////////////////////////////////////////////////////
// AOrbPath
AOrbPath::AOrbPath()
{
	this->Tags.Add(TEXT("OrbPath"));
	this->bSpawnOrbGroupAtStartup = false;
	this->OrbPath = CreateDefaultSubobject<USplineComponent>(TEXT("OrbPath"));
	
	if (IsValid(this->OrbPath))
	{
		this->OrbPath->AttachTo(this->RootComponent);
	}
}

///////////////////////////////////////////////////////////////////////////
// UE4 Native Events

void AOrbPath::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < this->SplinePointInfo.Num(); i++)
	{
		this->SplinePointInfo[i].bHasBeenVisited = false;
	}

	if (this->bSpawnOrbGroupAtStartup)
	{
		AActor* entity = AEntity::NewActorFromString(this, TEXT("/Game/Origami/Blueprints/Actors/"), TEXT("Bt_Act_OrbGroup.Bt_Act_OrbGroup"), false);
		if (!IsValid(entity))
		{
			UE_LOG(LogTemp, Error, TEXT("Couldn't create a new instance of Bt_Act_OrbGroup blueprint!"));
			return;
		}

		AttachedOrbGroup = Cast<AOrbGroup>(entity);
		if (IsValid(AttachedOrbGroup)) 
		{
			AttachedOrbGroup->AttachSocket(this);
		}
	}
}


///////////////////////////////////////////////////////////////////////////
// OrbPath Methods

FSplinePointInfo* AOrbPath::IsNearSplinePoint(float currentDistance, float maxDistance)
{
	if (!IsValid(this->OrbPath))
		return NULL;

	for (int i = 0; i < this->SplinePointInfo.Num(); i++) 
	{
		FSplinePointInfo& info = this->SplinePointInfo[i];

		if (info.bHasBeenVisited)
			continue;

		if (info.PointIndex >= this->OrbPath->GetNumSplinePoints())
			return NULL;

		float distance = this->OrbPath->GetDistanceAlongSplineAtSplinePoint(info.PointIndex);
		float deltaDistance = (currentDistance - distance);
		if (deltaDistance <= maxDistance)
			return &info;
	}

	return NULL;
}

void AOrbPath::DetachFromGroup()
{
	this->AttachedOrbGroup = NULL;
}


///////////////////////////////////////////////////////////////////////////
// Blueprint Exposed Methods

void AOrbPath::ResumePath() 
{
	if (IsValid(AttachedOrbGroup))
	{
		AttachedOrbGroup->K2_ResumeFromLocation();
	}
}
