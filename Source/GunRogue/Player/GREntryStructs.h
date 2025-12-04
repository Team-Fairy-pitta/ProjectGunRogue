// GREntryStructs.h

#pragma once

#include "CoreMinimal.h"
#include "GREntryStructs.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct FAugmentEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FName AugmentID;

	UPROPERTY()
	int32 Level;
};
