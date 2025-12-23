#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GRLowAmmoBaseMMC.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRLowAmmoBaseMMC : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UGRLowAmmoBaseMMC();

protected:
	bool IsHealthBelow(const FGameplayEffectSpec& Spec, float Threshold) const;

protected:
	FGameplayEffectAttributeCaptureDefinition AmmoDef;
	FGameplayEffectAttributeCaptureDefinition MaxAmmoDef;
};
