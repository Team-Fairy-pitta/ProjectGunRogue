#include "GRCriticalPowerUpEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"

UGRCriticalPowerUpEffect::UGRCriticalPowerUpEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UGRCombatAttributeSet::GetWeaponCriticalMultiplier_BonusAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SetByCallerMagnitude;
	SetByCallerMagnitude.DataTag = FGameplayTag::RequestGameplayTag("Weapon.Option.Value.CriticalDamage_Bouus");
	Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerMagnitude);

	Modifiers.Add(Mod);
}
