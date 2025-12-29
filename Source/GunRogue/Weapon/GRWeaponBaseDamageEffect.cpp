#include "Weapon/GRWeaponBaseDamageEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"

UGRWeaponBaseDamageEffect::UGRWeaponBaseDamageEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UGRCombatAttributeSet::GetWeaponDamage_BaseAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SetByCallerMagnitude;
	SetByCallerMagnitude.DataTag = FGameplayTag::RequestGameplayTag("Weapon.BaseDamage");
	Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerMagnitude);

	Modifiers.Add(Mod);
}
