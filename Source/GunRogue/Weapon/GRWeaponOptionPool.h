#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "GRWeaponOptionPool.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FOptionValueRange
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (Categories = "Weapon.Option"))
	FGameplayTag DataTag;

	UPROPERTY(EditAnywhere)
	FFloatInterval ValueRange;

	UPROPERTY(EditAnywhere)
	bool bIsPercentValue = false;

	UPROPERTY(EditAnywhere)
	bool bIsAdditivePercent = false;
};

USTRUCT(BlueprintType)
struct FOptionPoolEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGRGameplayEffect> EffectClass;

	UPROPERTY(EditAnywhere)
	TArray<FOptionValueRange> ValueRanges;

};

UCLASS()
class GUNROGUE_API UGRWeaponOptionPool : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TArray<FOptionPoolEntry> Options;
};
