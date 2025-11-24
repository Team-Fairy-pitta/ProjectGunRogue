#include "GRAttackPowerUpEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"

UGRAttackPowerUpEffect::UGRAttackPowerUpEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UGRCombatAttributeSet::GetWeaponDamage_AdditiveAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;

	Mod.ModifierMagnitude = FScalableFloat(0.f);

	Modifiers.Add(Mod);
}
