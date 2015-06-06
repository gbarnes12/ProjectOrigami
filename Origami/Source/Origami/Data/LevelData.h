#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "LevelData.generated.h"

USTRUCT(BlueprintType)
struct FLevelData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	/** The name of the level! */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Level)
	FName Level;

};