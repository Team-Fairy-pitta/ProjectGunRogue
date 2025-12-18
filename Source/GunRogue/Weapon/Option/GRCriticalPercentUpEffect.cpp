#include "GRCriticalPercentUpEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"

UGRCriticalPercentUpEffect::UGRCriticalPercentUpEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UGRCombatAttributeSet::GetWeaponCriticalMultiplier_MultiplicativeAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;

	Mod.ModifierMagnitude = FScalableFloat(0.f);

	Modifiers.Add(Mod);
}
