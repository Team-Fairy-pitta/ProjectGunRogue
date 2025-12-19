#include "Weapon/Option/ExecutionConditions/GRAmmoPercentExecution.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"


struct FLowAmmoStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentAmmo);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxAmmo);

	FLowAmmoStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(
			UGRCombatAttributeSet,
			CurrentAmmo,
			Source,
			false
		);

		DEFINE_ATTRIBUTE_CAPTUREDEF(
			UGRCombatAttributeSet,
			MaxAmmo,
			Source,
			false
		);
	}
};

static const FLowAmmoStatics& LowAmmoStatics()
{
	static FLowAmmoStatics Statics;

	return Statics;
}

UGRAmmoPercentExecution::UGRAmmoPercentExecution()
{
	RelevantAttributesToCapture.Add(LowAmmoStatics().CurrentAmmoDef);
	RelevantAttributesToCapture.Add(LowAmmoStatics().MaxAmmoDef);
}

bool UGRAmmoPercentExecution::CheckCondition(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FAggregatorEvaluateParameters& EvalParams) const
{
	float CurrentAmmo = 0.f;
	float MaxAmmo = 0.f;

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(LowAmmoStatics().CurrentAmmoDef, EvalParams, CurrentAmmo);

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(LowAmmoStatics().MaxAmmoDef, EvalParams, MaxAmmo);

	if (MaxAmmo <= 0.f)
	{
		return false;
	}

	const float AmmoPercent = CurrentAmmo / MaxAmmo;
	const float Threshold = ExecutionParams.GetOwningSpec().GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.AmmoThreshold"),	false, 0.f);

	return AmmoPercent <= Threshold;
}

void UGRAmmoPercentExecution::ApplyResult(const FGameplayEffectSpec& Spec, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const float DamageBonus = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.DamageBonus"), false, 0.f);

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UGRCombatAttributeSet::GetWeaponDamage_MultiplicativeAttribute(), EGameplayModOp::MultiplyAdditive,	DamageBonus));
}
