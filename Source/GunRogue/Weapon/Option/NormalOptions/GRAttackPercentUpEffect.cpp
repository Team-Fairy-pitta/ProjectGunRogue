#include "GRAttackPercentUpEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"

UGRAttackPercentUpEffect::UGRAttackPercentUpEffect ()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UGRCombatAttributeSet::GetWeaponDamage_MultiplicativeAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SetByCallerMagnitude;
	SetByCallerMagnitude.DataTag = FGameplayTag::RequestGameplayTag("Weapon.Option.Value.Damage_Multiplicative");
	Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerMagnitude);

	Modifiers.Add(Mod);
}
