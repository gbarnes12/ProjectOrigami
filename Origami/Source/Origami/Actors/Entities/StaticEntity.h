#pragma once

#include "Actors/Entity.h"
#include "StaticEntity.generated.h"

UCLASS()
class ORIGAMI_API AStaticEntity : public AEntity
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Visual, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;

public:
	AStaticEntity();


	virtual void Dismantle() override;
	virtual void Mantle() override;
};