// GRPerkStructs.h

#pragma once

#include "CoreMinimal.h"
#include "GRPerkStructs.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FPerkEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FName PerkID;

	UPROPERTY()
	int32 Level;
};
