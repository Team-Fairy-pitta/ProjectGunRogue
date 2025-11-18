// PerkLevel.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PerkLevel.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FPerkLevel : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PerkName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxLevel = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ValuePerLevel = 0.f;
};
