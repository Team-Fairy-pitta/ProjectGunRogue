#include "Weapon/Option/MMC/GRLowAmmoAttackUpMMC.h"

float UGRLowAmmoAttackUpMMC::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float Threshold = Spec.GetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag("Weapon.Option.Threshold.CurrentHealth"),
		false,
		0.f
	);

	if (!IsHealthBelow(Spec, Threshold))
	{
		return 0.f;
	}



	float Value = Spec.GetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag("Weapon.Option.Value.Damage_Multiplicative"),
		false,
		0.f
	);

	return Value;
}
