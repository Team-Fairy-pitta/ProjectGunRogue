#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GRWeaponOptionPool.h"
#include "GRWeaponDataAsset.generated.h"


UCLASS()
class GUNROGUE_API UGRWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseDamage = 10.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float UpgradeDamageIncrease = 2.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UGRWeaponOptionPool* OptionPool = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float UpgradeSeccessProbability = 0.8f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 UpgradeCost = 1;
};
