#include "GRFireSpeedPercentUpEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"


UGRFireSpeedPercentUpEffect::UGRFireSpeedPercentUpEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UGRCombatAttributeSet::GetFireRateAttribute();
	Mod.ModifierOp = EGameplayModOp::MultiplyAdditive;

	Mod.ModifierMagnitude = FScalableFloat(0.f);

	Modifiers.Add(Mod);
}
