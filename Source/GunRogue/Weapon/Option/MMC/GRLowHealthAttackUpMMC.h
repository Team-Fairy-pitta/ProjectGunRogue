#pragma once

#include "CoreMinimal.h"
#include "Weapon/Option/MMC/GRLowHealthBaseMMC.h"
#include "GRLowHealthAttackUpMMC.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRLowHealthAttackUpMMC : public UGRLowHealthBaseMMC
{
	GENERATED_BODY()
protected:
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
