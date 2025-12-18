#include "GRAttackPercentUpEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"

UGRAttackPercentUpEffect::UGRAttackPercentUpEffect ()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UGRCombatAttributeSet::GetWeaponDamage_MultiplicativeAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;

	Mod.ModifierMagnitude = FScalableFloat(0.f);

	Modifiers.Add(Mod);
}
