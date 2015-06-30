#pragma once
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"

#include "Runtime/Engine/Classes/Components/TimelineComponent.h"
#include "OrbGroup.generated.h"

enum EOrbMode {
	FreeRoam = 0,
	Attached = 1
};

enum EActorType {
	None = 0,
	Player = 1,
	Path = 2,
	Entity = 3
};

struct FOrbGroupTargetInfo
{
public:
	class AActor* Target;
	struct FVector StartLocation;
	struct FVector TargetLocation;
	bool bAttachToTargetAtEnd;
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
	class AActor* Socket;

public:
	/*UE4 Native Events*/
	virtual void BeginPlay() override;

	/*UE4 Native Event*/
	virtual void Tick(float deltaSeconds) override;

	/*Attach a new socket to the orb group!*/
	void AttachSocket(AActor* socket);

	/*Detach from socket. */
	void DetachFromSocket();

	/*This will detach you from your current socket and will move towards the target. */
	void StartMoveToTarget(AActor* target, bool bAttachToTargetAtEnd = false);

	/*This will detach you from your current socket and will move towards the target. When Actor target is null it won't attach to the target at the end but will move to the passed location.*/
	void StartMoveToTarget(AActor* target, FVector location, bool bAttachToTargetAtEnd = false);

	/*This will change the color of the orbs*/
	void ChangeColor(FColor color);

public:

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Static Mesh Socket", Keywords = "static mesh socket"), Category = StaticMesh)
	static UStaticMeshSocket* GetStaticMeshSocket(UStaticMesh* StaticMesh, const FName SocketName);

private:
	/* The scene root component from which everything originates */
	class USceneComponent* RootSceneComponent;

	/* This is used as a temporary reference to the path we're attached to! */
	class USplineComponent* OrbPath;

	/* Pointer to the static mesh we use for the orb mesh */
	class UStaticMesh* OrbMesh;

	/* Pointer to the light component for the orbs. */
	class UPointLightComponent* Light;

	/* Pointer to the orb material instance in order to modify parameters at runtime. */
	class UMaterialInstanceDynamic* OrbMaterialInstance;

	/* Handle to the timer that will adjust the speed periodically */
	struct FTimerHandle AdjustSpeedTimerHandle;

	/* The timeline can be played whenever the orb group should move towards a target which isn't controlled by a spline! */
	struct FTimeline MovementTimeline;

	/*Target information*/
	struct FOrbGroupTargetInfo TargetInfo;

	/* To which actor are we attached? Entity, Player or OrbPath? */
	EActorType AttachedType;
	
	/* In which mode are we. This determines the simulation */
	EOrbMode Mode;

	/*How fast are we currently moving.*/
	float MovementSpeed;
	
	/*How much of the path have we already travelled?*/
	float TravelledDistanceOnPath;
	
	/*Are we finished generating*/
	bool bIsGenerated;

	/*Which is the current rotation?*/
	float CurrentRotation;

private:
	/* Construction of the orbs */
	void GenerateOrbs();

	/* Simulate the stuff we need to do once we are attached to another actor! */
	void SimulateAttachment(float deltaSeconds);

	/*Simulate free roaming when we aren't attached!*/
	void SimulateRoaming(float deltaSeconds);

	/* Once we are attached to a socket containing a path. Follow it ;) */
	void FollowPath(float deltaSeconds);

	/* Timer based event methods */
	void AdjustSpeed();

	/* Move to target based on a given timeline */
	UFUNCTION()
	void MoveToTarget(float value);
};