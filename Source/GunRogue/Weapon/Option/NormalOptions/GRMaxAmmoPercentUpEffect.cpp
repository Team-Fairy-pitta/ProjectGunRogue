#include "GRMaxAmmoPercentUpEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"


UGRMaxAmmoPercentUpEffect::UGRMaxAmmoPercentUpEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UGRCombatAttributeSet::GetMaxAmmoAttribute();
	Mod.ModifierOp = EGameplayModOp::MultiplyAdditive;

	FSetByCallerFloat SetByCallerMagnitude;
	SetByCallerMagnitude.DataTag = FGameplayTag::RequestGameplayTag("Weapon.Option.Value.MaxAmmo");
	Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerMagnitude);

	Modifiers.Add(Mod);
}
