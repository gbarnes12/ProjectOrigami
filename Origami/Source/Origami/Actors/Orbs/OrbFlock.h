#pragma once
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "Runtime/Engine/Classes/Components/InstancedStaticMeshComponent.h"
#include "OrbFlock.generated.h"

//#define USE_OLD_COLLISION

struct FOrbFlockMember
{
public:
	int MeshInstanceId;
	bool bIsLeader;
	FTransform Transform;
	FVector Velocity;
	FVector Target;
	float ElapsedTimeSinceTargetUpdate;
	UParticleSystemComponent* Trail;

	FOrbFlockMember()
	{
		MeshInstanceId = 0;
		bIsLeader = false;
		Velocity = FVector::ZeroVector;
		Target = FVector::ZeroVector;
		ElapsedTimeSinceTargetUpdate = 0;
		Trail = nullptr;
	};

	FVector ComputeAlignment(TArray<FOrbFlockMember>& member, float maxSpeed, float neighborRadius, float maxForce);
	FVector ComputeSeparation(TArray<FOrbFlockMember>& member, float maxSpeed, float neighborRadius, float maxForce);
	FVector ComputeCohesion(TArray<FOrbFlockMember>& member, float maxSpeed, float neighborRadius, float maxForce);
	FVector ComputeSteerTo(FVector Target, float maxSpeed, float maxForce);
	FVector ComputeAvoidance(AActor* actor, UWorld* world, FVector Target, float maxSpeed, float maxForce);
};

USTRUCT()
struct FFlockDebugVisualizationSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
		bool bShowDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		bool bShowAlignment = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		bool bShowSeparation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		bool bShowCohesion = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		bool bShowTarget = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		bool bShowVelocity = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		float ArrowScale = 2.0f;
};

USTRUCT()
struct FFlockVisualSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual", meta = (ClampMin = "1"))
		int32 OrbCount = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual", meta = (ClampMin = "0.0"))
		float OrbScales = 0.1f;
};

USTRUCT() 
struct FFlockSimulationSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		bool bMoveActor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation", meta = (ClampMin = "0.0"))
		float NeighborRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation", meta = (ClampMin = "0.0"))
		float SeparationRadius = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation", meta = (ClampMin = "0.0"))
		float FlockMaxSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation", meta = (ClampMin = "0.0"))
		float CohesionWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation", meta = (ClampMin = "0.0"))
		float SeparationWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation", meta = (ClampMin = "0.0"))
		float AlignmentWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation", meta = (ClampMin = "0.0"))
		float SteerToTargetWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation", meta = (ClampMin = "0.0"))
		float MaxForce = 0.005f;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock")
	FFlockDebugVisualizationSettings Debug;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flock")
	UInstancedStaticMeshComponent* StaticMeshInstanceComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flock")
	UStaticMesh* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flock")
	USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere, Category = Behavior)
	class UBehaviorTree* OrbBehavior;

	UPROPERTY(EditAnywhere, Category = Behavior)
	class UBlackboardData* BlackboardAsset;

	UPROPERTY(EditAnywhere, Category = "Particle")
	class UParticleSystem* ParticleSystem;

	class AOrbAiController* AiController;
	
public:
	AOrbFlock(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Flock")
	FVector CalculateNewTarget();

	UFUNCTION(BlueprintCallable, Category = "Flock")
	bool IsLeaderAtLocation(FVector location, float thresholdDistance);

private:
	/* The scene root component from which everything originates */
	class USceneComponent* RootSceneComponent;
	class UPointLightComponent* Light;
	TArray<FOrbFlockMember> Orbs;
	FVector TempRealTarget; 
	bool bCollidedWithObject;
	

	virtual void BeginPlay() override;
	virtual void Tick(float deltaSeconds) override;

	
	void AddFlockMember(const FTransform& transform, bool bIsLeader = false);
	void SimulateOrbMember(float deltaSeconds, FOrbFlockMember& member);
	void DrawDebugInformation(FOrbFlockMember& member, FVector cohesion, FVector separation, FVector alignment);

	FVector GetRandomTarget();
	FRotator FindLookAtRotation(FVector start, FVector end);

};