// GRAugmentStructs.h

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GRAugmentStructs.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FAugmentValues
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AugmentTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> ValuePerLevel;
};

USTRUCT(BlueprintType)
struct FAugmentAdditionalValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AugmentAdditionalTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText AugmentAdditionalDescription;
};
