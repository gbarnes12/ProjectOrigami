#pragma once

#include "Actors/Entity.h"
#include "StaticEntity.generated.h"

UCLASS()
class ORIGAMI_API AStaticEntity : public AEntity
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StaticEntity, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;

public:
	AStaticEntity();

public:
	/** UE4 native events*/
	virtual void BeginPlay() override;

public:
	/** Override of entity methods */
	virtual void Dismantle() override;
	virtual void Mantle() override;
};