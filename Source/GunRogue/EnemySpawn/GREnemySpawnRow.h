#pragma once

#include "Engine/DataTable.h"
#include "GREnemySpawnRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FGREnemySpawnRow : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EnemyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnChance = 0.0f;

};


