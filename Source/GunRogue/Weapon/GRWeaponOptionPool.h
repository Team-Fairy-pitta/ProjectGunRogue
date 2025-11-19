#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "GRWeaponOptionPool.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FOptionPoolEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGRGameplayEffect> EffectClass;

	UPROPERTY(EditAnywhere)
	float MinValue = 1.f;

	UPROPERTY(EditAnywhere)
	float MaxValue = 5.f;

};

UCLASS()
class GUNROGUE_API UGRWeaponOptionPool : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TArray<FOptionPoolEntry> Options;
};
