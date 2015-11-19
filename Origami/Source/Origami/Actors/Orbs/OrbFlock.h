#pragma once
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/Engine/Classes/Components/InstancedStaticMeshComponent.h"
#include "OrbFlock.generated.h"

struct FOrbFlockMember
{
public:
	int MeshInstanceId;
	bool bIsLeader;
	FTransform Transform;
	FVector Velocity;
	FVector Target;
	float ElapsedTimeSinceTargetUpdate;

	FOrbFlockMember()
	{
		MeshInstanceId = 0;
		bIsLeader = false;
		Velocity = FVector::ZeroVector;
		Target = FVector::ZeroVector;
		ElapsedTimeSinceTargetUpdate = 0;
	};
};

USTRUCT()
struct FFlockVisualSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
		int32 OrbCount = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
		float OrbScales = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
		float FlockWanderUpdateRate = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
		float TargetMinThreshold = 200.0f;
};

USTRUCT() 
struct FFlockSimulationSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		bool bUseCollisionAvoidance = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		float FlockMinSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		float FlockMaxSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		float CohesionWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		float SeparationWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		float AlignmentWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		float FollowWeight = 1.0f;
};


UCLASS(config = Game)
class ORIGAMI_API AOrbFlock : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock")
	FFlockVisualSettings Visual;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock")
	FFlockSimulationSettings Simulation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flock")
	UInstancedStaticMeshComponent* StaticMeshInstanceComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock")
	UStaticMesh* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flock")
	USphereComponent* SphereComponent;
	
	AOrbFlock(const FObjectInitializer& ObjectInitializer);

private:
	/* The scene root component from which everything originates */
	class USceneComponent* RootSceneComponent;
	TArray<FOrbFlockMember> Orbs;
	

	virtual void BeginPlay() override;
	virtual void Tick(float deltaSeconds) override;

	void CalculateNewTarget();
	void AddFlockMember(const FTransform& transform, bool bIsLeader = false);
	FVector GetRandomTarget();
	FRotator FindLookAtRotation(FVector start, FVector end);
	FVector ComputeAvoidance(FOrbFlockMember& member, FVector velocity, FVector target);
	FVector ComputeWanderVelocity(FOrbFlockMember& member);
	FVector ComputeAlignment(FOrbFlockMember& member);
	FVector ComputSeparation(FOrbFlockMember& member);
	FVector ComputeCohesion(FOrbFlockMember& member);
	FVector ComputeFollow(FOrbFlockMember& member);
};