#include "GRAttackPowerUpEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"

UGRAttackPowerUpEffect::UGRAttackPowerUpEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UGRCombatAttributeSet::GetWeaponDamage_AdditiveAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SetByCallerMagnitude;
	SetByCallerMagnitude.DataTag = FGameplayTag::RequestGameplayTag("Weapon.Option.Value.Damage_Additive");
	Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerMagnitude);

	Modifiers.Add(Mod);
}
