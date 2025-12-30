#include "Weapon/Option/MMC/GRLowAmmoBaseMMC.h"

#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "AbilitySystemComponent.h"


UGRLowAmmoBaseMMC::UGRLowAmmoBaseMMC()
{
	AmmoDef.AttributeToCapture = UGRCombatAttributeSet::GetCurrentAmmoAttribute();
	AmmoDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target; 
	AmmoDef.bSnapshot = false;

	MaxAmmoDef.AttributeToCapture = UGRCombatAttributeSet::GetMaxAmmoAttribute();
	MaxAmmoDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target; 
	MaxAmmoDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(AmmoDef);
	RelevantAttributesToCapture.Add(MaxAmmoDef);
}

bool UGRLowAmmoBaseMMC::IsHealthBelow(const FGameplayEffectSpec& Spec, float Threshold) const
{
	float Ammo = 0.f;
	float MaxAmmo = 1.f;

	const bool bHasAmmo = GetCapturedAttributeMagnitude(AmmoDef, Spec, {}, Ammo);
	const bool bHasMaxAmmo = GetCapturedAttributeMagnitude(MaxAmmoDef, Spec, {}, MaxAmmo);

	if (!bHasAmmo || !bHasMaxAmmo || MaxAmmo <= 0.f)
	{
		return false;
	}

	return (Ammo / MaxAmmo) <= Threshold;
}
