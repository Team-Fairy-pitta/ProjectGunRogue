#pragma once

#include "CoreMinimal.h"
#include "Weapon/Option/MMC/GRLowAmmoBaseMMC.h"
#include "GRLowAmmoAttackUpMMC.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRLowAmmoAttackUpMMC : public UGRLowAmmoBaseMMC
{
	GENERATED_BODY()
	
protected:
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

};
