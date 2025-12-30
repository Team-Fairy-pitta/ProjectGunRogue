#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GRLowHealthBaseMMC.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRLowHealthBaseMMC : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UGRLowHealthBaseMMC();

protected:
	bool IsHealthBelow(const FGameplayEffectSpec& Spec, float Threshold) const;

protected:
	FGameplayEffectAttributeCaptureDefinition HealthDef;
	FGameplayEffectAttributeCaptureDefinition MaxHealthDef;
};
