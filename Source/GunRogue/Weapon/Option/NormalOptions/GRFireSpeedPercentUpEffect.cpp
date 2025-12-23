#include "GRFireSpeedPercentUpEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"


UGRFireSpeedPercentUpEffect::UGRFireSpeedPercentUpEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UGRCombatAttributeSet::GetFireRateAttribute();
	Mod.ModifierOp = EGameplayModOp::MultiplyAdditive;

	FSetByCallerFloat SetByCallerMagnitude;
	SetByCallerMagnitude.DataTag = FGameplayTag::RequestGameplayTag("Weapon.Option.Value.FireRate");
	Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerMagnitude);

	Modifiers.Add(Mod);
}
