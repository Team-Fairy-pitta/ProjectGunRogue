#include "Weapon/Option/MMC/GRLowHealthBaseMMC.h"

#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "AbilitySystemComponent.h"


UGRLowHealthBaseMMC::UGRLowHealthBaseMMC()
{
	HealthDef.AttributeToCapture = UGRHealthAttributeSet::GetHealthAttribute();
	HealthDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target; 
	HealthDef.bSnapshot = false;

	MaxHealthDef.AttributeToCapture = UGRHealthAttributeSet::GetMaxHealthAttribute();
	MaxHealthDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target; 
	MaxHealthDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(HealthDef);
	RelevantAttributesToCapture.Add(MaxHealthDef);
}

bool UGRLowHealthBaseMMC::IsHealthBelow(const FGameplayEffectSpec& Spec, float Threshold) const
{
	float HP = 0.f;
	float MaxHP = 1.f;

	const bool bHasHP = GetCapturedAttributeMagnitude(HealthDef, Spec, {}, HP);
	const bool bHasMaxHP = GetCapturedAttributeMagnitude(MaxHealthDef, Spec, {}, MaxHP);

	if (!bHasHP || !bHasMaxHP || MaxHP <= 0.f)
	{
		return false;
	}

	return (HP / MaxHP) <= Threshold;
}
