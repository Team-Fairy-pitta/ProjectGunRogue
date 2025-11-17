#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "Net/UnrealNetwork.h"

UGRCombatAttributeSet::UGRCombatAttributeSet()
{
	// 기본값 설정
	WeaponDamage_Base = 10.0f;
	WeaponDamage_Additive = 0.0f;
	WeaponDamage_Multiplicative = 0.0f;

	WeaponCriticalMultiplier_Base = 2.0f;
	WeaponCriticalMultiplier_Multiplicative = 0.0f;
	WeaponCriticalMultiplier_Bonus = 0.0f;
	WeaponNormalMultiplier_Multiplicative = 0.0f;

	FinalDamage_Additive = 0.0f;
	FinalDamage_Multiplicative = 0.0f;
	FinalDamage_Bonus = 0.0f;

	DamageReduction = 0.0f;

	IsCriticalHit = 0.0f;
}

void UGRCombatAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, WeaponDamage_Base, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, WeaponDamage_Additive, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, WeaponDamage_Multiplicative, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, WeaponCriticalMultiplier_Base, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, WeaponCriticalMultiplier_Multiplicative, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, WeaponCriticalMultiplier_Bonus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, WeaponNormalMultiplier_Multiplicative, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, FinalDamage_Additive, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, FinalDamage_Multiplicative, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, FinalDamage_Bonus, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, DamageReduction, COND_None, REPNOTIFY_Always);
}

float UGRCombatAttributeSet::CalculateWeaponDamage() const
{
	// [무기 공격력] = (기본 공격력 + 증가) × (1 + 증폭)
	const float Base = GetWeaponDamage_Base();
	const float Additive = GetWeaponDamage_Additive();
	const float Multiplicative = GetWeaponDamage_Multiplicative();

	const float WeaponDamage = (Base + Additive) * (1.0f + Multiplicative);

	return WeaponDamage;
}

float UGRCombatAttributeSet::CalculateCriticalMultiplier(bool bIsCritical) const
{
	if (bIsCritical)
	{
		// [약점 배율] = (기본 약점 배율 × (1 + 증폭)) + 추가 증가
		const float Base = GetWeaponCriticalMultiplier_Base();
		const float Multiplicative = GetWeaponCriticalMultiplier_Multiplicative();
		const float Bonus = GetWeaponCriticalMultiplier_Bonus();

		const float CritMultiplier = (Base * (1.0f + Multiplicative)) + Bonus;

		return CritMultiplier;
	}
	else
	{
		// [일반 배율] = (1 + 증폭 또는 감쇠)
		const float Multiplicative = GetWeaponNormalMultiplier_Multiplicative();

		const float NormalMultiplier = 1.0f + Multiplicative;

		return NormalMultiplier;
	}
}

float UGRCombatAttributeSet::CalculateFinalDamageMultiplier() const
{
	// [최종 피해 배율] = (1 + 증가) × (1 + 증폭) + 추가 증가
	const float Additive = GetFinalDamage_Additive();
	const float Multiplicative = GetFinalDamage_Multiplicative();
	const float Bonus = GetFinalDamage_Bonus();

	const float FinalMultiplier = ((1.0f + Additive) * (1.0f + Multiplicative)) + Bonus;

	return FinalMultiplier;
}

float UGRCombatAttributeSet::CalculateFinalWeaponDamage(bool bIsCritical, float TargetDamageReduction) const
{
	// 공식: [무기 공격력] × [약점 배율] × [최종 피해 배율] × [1 - 피해 감소]
	const float WeaponDamage = CalculateWeaponDamage();
	const float CritMultiplier = CalculateCriticalMultiplier(bIsCritical);
	const float FinalMultiplier = CalculateFinalDamageMultiplier();

	const float FinalDamage = WeaponDamage * CritMultiplier * FinalMultiplier * (1.0f - TargetDamageReduction);

	UE_LOG(LogTemp, Log, TEXT("[Damage Calc] Weapon: %.1f, Crit: %.2f, Final: %.2f, Reduction: %.2f => Result: %.1f"),
		WeaponDamage, CritMultiplier, FinalMultiplier, TargetDamageReduction, FinalDamage);

	return FMath::Max(FinalDamage, 0.0f);
}

// OnRep 함수들
void UGRCombatAttributeSet::OnRep_WeaponDamage_Base(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, WeaponDamage_Base, OldValue);
}

void UGRCombatAttributeSet::OnRep_WeaponDamage_Additive(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, WeaponDamage_Additive, OldValue);
}

void UGRCombatAttributeSet::OnRep_WeaponDamage_Multiplicative(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, WeaponDamage_Multiplicative, OldValue);
}

void UGRCombatAttributeSet::OnRep_WeaponCriticalMultiplier_Base(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, WeaponCriticalMultiplier_Base, OldValue);
}

void UGRCombatAttributeSet::OnRep_WeaponCriticalMultiplier_Multiplicative(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, WeaponCriticalMultiplier_Multiplicative, OldValue);
}

void UGRCombatAttributeSet::OnRep_WeaponCriticalMultiplier_Bonus(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, WeaponCriticalMultiplier_Bonus, OldValue);
}

void UGRCombatAttributeSet::OnRep_WeaponNormalMultiplier_Multiplicative(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, WeaponNormalMultiplier_Multiplicative, OldValue);
}

void UGRCombatAttributeSet::OnRep_FinalDamage_Additive(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, FinalDamage_Additive, OldValue);
}

void UGRCombatAttributeSet::OnRep_FinalDamage_Multiplicative(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, FinalDamage_Multiplicative, OldValue);
}

void UGRCombatAttributeSet::OnRep_FinalDamage_Bonus(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, FinalDamage_Bonus, OldValue);
}

void UGRCombatAttributeSet::OnRep_DamageReduction(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, DamageReduction, OldValue);
}
