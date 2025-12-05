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

	// 변수 구분용 태그 (Ex: 데미지, 범위 등)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AugmentTag;

	// 변수의 레벨 당 값 (Ex: 20%/50%/100% 등) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> ValuePerLevel;
};

USTRUCT(BlueprintType)
struct FAugmentAdditionalValue
{
	GENERATED_BODY()

	// 3레벨 시 추가되는 변수 구분용 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AugmentAdditionalTag;

	//추가되는 변수 설명 (UI 용도)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText AugmentAdditionalDescription;

	//추가되는 변수의 값 (필요 시)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AugmentAdditionalValue;
};

USTRUCT(BlueprintType)
struct FAugmentEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FName AugmentID;

	UPROPERTY()
	int32 Level;
};
