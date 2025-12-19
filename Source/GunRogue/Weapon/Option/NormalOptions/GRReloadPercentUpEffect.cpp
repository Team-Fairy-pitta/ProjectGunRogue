#include "GRReloadPercentUpEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"

UGRReloadPercentUpEffect::UGRReloadPercentUpEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UGRCombatAttributeSet::GetReloadRateAttribute();
	Mod.ModifierOp = EGameplayModOp::MultiplyAdditive;

	Mod.ModifierMagnitude = FScalableFloat(0.f);

	Modifiers.Add(Mod);
}
