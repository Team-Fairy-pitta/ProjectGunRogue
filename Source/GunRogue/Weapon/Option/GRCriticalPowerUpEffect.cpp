#include "GRCriticalPowerUpEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"

UGRCriticalPowerUpEffect::UGRCriticalPowerUpEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UGRCombatAttributeSet::GetWeaponCriticalMultiplier_BonusAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;

	Mod.ModifierMagnitude = FScalableFloat(0.f);

	Modifiers.Add(Mod);
}
