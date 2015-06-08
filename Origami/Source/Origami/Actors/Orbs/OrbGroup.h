#pragma once
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
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
	TArray<float> OrbSpeedLevel;

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

	/* Pointer to the static mesh we use for the orb mesh */
	class UStaticMesh* OrbMesh;

	/* Pointer to the orb material instance in order to modify parameters at runtime. */
	class UMaterialInstanceDynamic* OrbMaterialInstance;

	/* Handle to the timer that will adjust the speed periodically */
	struct FTimerHandle AdjustSpeedTimerHandle;

	/* The mode this orb group is currently in. */
	EOrbMode Mode;
	float MovementSpeed;
	float TravelledDistanceOnPath;
	bool bIsGenerated;

	void GenerateOrbs();

	/* Timer based event methods */
	void AdjustSpeed();
};