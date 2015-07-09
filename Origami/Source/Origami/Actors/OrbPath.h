#pragma once
#include "GameFramework/Actor.h"
#include "OrbPath.generated.h"

USTRUCT(BlueprintType)
struct FSplinePointInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "Spline Point Info")
	bool bWaitAtPoint;

	UPROPERTY(EditAnywhere, Category = "Spline Point Info")
	float TimeToWait;

	UPROPERTY(EditAnywhere, Category = "Spline Point Info")
	bool bPauseAtPoint;

	UPROPERTY(EditAnywhere, Category = "Spline Point Info")
	int32 PointIndex;

	bool bHasBeenVisited;

	FSplinePointInfo()
	{
		bHasBeenVisited = false;
		bWaitAtPoint = false;
		bPauseAtPoint = false;
		TimeToWait = 10.0f;
		PointIndex = 0; 
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotifyOrbsEndReachedEvent, AOrbGroup*, OrbGroup);

UCLASS(config = Game)
class ORIGAMI_API AOrbPath : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = OrbPath)
	class USplineComponent* OrbPath;
	
	UPROPERTY(EditAnywhere, Category = OrbPath)
	TArray<struct FSplinePointInfo> SplinePointInfo;

	UPROPERTY(BlueprintAssignable, Category = OrbPath)
	FNotifyOrbsEndReachedEvent OrbsHaveReachedEnd;

	UPROPERTY(EditAnywhere, Category = OrbPath)
	bool bSpawnOrbGroupAtStartup;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = OrbPath)
	class AOrbGroup* AttachedOrbGroup;

public:
	AOrbPath();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = OrbPath)
	void ResumePath();

	void DetachFromGroup();

	FSplinePointInfo* IsNearSplinePoint(float currentDistance, float maxDistance);
};