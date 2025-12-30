#include "GRReloadPercentUpEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"

UGRReloadPercentUpEffect::UGRReloadPercentUpEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UGRCombatAttributeSet::GetReloadRateAttribute();
	Mod.ModifierOp = EGameplayModOp::MultiplyAdditive;

	FSetByCallerFloat SetByCallerMagnitude;
	SetByCallerMagnitude.DataTag = FGameplayTag::RequestGameplayTag("Weapon.Option.Value.ReloadRate");
	Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerMagnitude);

	Modifiers.Add(Mod);
}
