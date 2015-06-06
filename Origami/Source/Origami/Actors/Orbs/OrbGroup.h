#pragma once
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "OrbGroup.generated.h"

enum EOrbMode {
	Swarm = 0,
	SeekAim = 1,
	Attached = 2
};


UCLASS(config = Game)
class AOrbGroup : public AActor
{
	GENERATED_BODY()

public:
	AOrbGroup(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category = Visual)
	int32 OrbCount;

	UPROPERTY(EditAnywhere, Category = Visual)
	FColor OrbColor;

	UPROPERTY(EditAnywhere, Category = Visual)
	FString OrbMeshFileName;

	UPROPERTY(EditAnywhere, Category = Visual)
	float OrbSpawnBoxExtents;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Visual, meta = (AllowPrivateAccess = "true"))
	class USplineComponent* OrbPath;


	void BeginPlay();
	void Tick(float deltaSeconds);

private:
	/* The scene root component from which everything originates */
	class USceneComponent* RootSceneComponent;
	/* This component holds all the orbs and is used to move them as a unit. */
	class USceneComponent* OrbsSceneComponent;

	float TravelledDistanceOnPath;
	EOrbMode Mode;
	bool bIsGenerated;

	void GenerateOrbs();
};