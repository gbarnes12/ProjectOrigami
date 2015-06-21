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

enum EActorType {
	None = 0,
	Player = 1,
	Path = 2,
	Entity = 3
};

UCLASS(config = Game)
class ORIGAMI_API AOrbGroup : public AActor
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Visual, meta = (AllowPrivateAccess = "true"))
	class AActor* Socket;

	virtual void BeginPlay() override;
	virtual void Tick(float deltaSeconds) override;

	void AttachSocket(AActor* socket);
	void DetachFromSocket();

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Static Mesh Socket", Keywords = "static mesh socket"), Category = StaticMesh)
	static UStaticMeshSocket* GetStaticMeshSocket(UStaticMesh* StaticMesh, const FName SocketName);

	
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
	EActorType AttachedType;
	EOrbMode Mode;
	float MovementSpeed;
	float TravelledDistanceOnPath;
	bool bIsGenerated;
	float CurrentRotation;

	/* Construction of the orbs */
	void GenerateOrbs();

	/* Simulation Methods */
	void FollowPath(float deltaSeconds);
	void SimulateSwarm(float deltaSeconds);

	/* Timer based event methods */
	void AdjustSpeed();
};