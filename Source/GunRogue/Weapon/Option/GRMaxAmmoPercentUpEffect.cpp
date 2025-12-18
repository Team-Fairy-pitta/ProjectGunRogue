#include "GRMaxAmmoPercentUpEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"


UGRMaxAmmoPercentUpEffect::UGRMaxAmmoPercentUpEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UGRCombatAttributeSet::GetMaxAmmoAttribute();
	Mod.ModifierOp = EGameplayModOp::MultiplyAdditive;

	Mod.ModifierMagnitude = FScalableFloat(0.f);

	Modifiers.Add(Mod);
}
