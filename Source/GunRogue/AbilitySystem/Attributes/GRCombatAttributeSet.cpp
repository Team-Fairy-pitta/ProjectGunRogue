#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "Net/UnrealNetwork.h"

UGRCombatAttributeSet::UGRCombatAttributeSet()
{
	// 기본값 설정
	WeaponBaseDamage = 10.0f;
	WeaponDamageIncrease = 0.0f;
	WeaponDamageMultiplier = 0.0f;

	BaseCriticalMultiplier = 2.0f;  // 200% (기본 약점 배율)
	CriticalMultiplierAmplify = 0.0f;
	CriticalMultiplierAdditional = 0.0f;
	NormalMultiplier = 0.0f;  // 1.0 (일반 배율)

	FinalDamageIncrease = 0.0f;
	FinalDamageMultiplier = 0.0f;
	FinalDamageAdditional = 0.0f;

	DamageReduction = 0.0f;

	IsCriticalHit = 0.0f;
}

void UGRCombatAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, WeaponBaseDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, WeaponDamageIncrease, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, WeaponDamageMultiplier, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, BaseCriticalMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, CriticalMultiplierAmplify, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, CriticalMultiplierAdditional, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, NormalMultiplier, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, FinalDamageIncrease, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, FinalDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, FinalDamageAdditional, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, DamageReduction, COND_None, REPNOTIFY_Always);
}

float UGRCombatAttributeSet::CalculateFinalDamage(bool bIsCritical, float TargetDamageReduction) const
{
	// 1. 무기 공격력 계산
	// [무기 공격력] = (기본 공격력 + 증가) × (1 + 증폭)
	float WeaponDamage = (GetWeaponBaseDamage() + GetWeaponDamageIncrease())
		* (1.0f + GetWeaponDamageMultiplier());

	// 2. 약점 배율 계산
	float CritMultiplier;
	if (bIsCritical)
	{
		// [약점 배율] = (기본 약점 배율 × (1 + 증폭)) + 추가 증가
		CritMultiplier = (GetBaseCriticalMultiplier() * (1.0f + GetCriticalMultiplierAmplify()))
			+ GetCriticalMultiplierAdditional();
	}
	else
	{
		// [일반 배율] = (1 + 증폭 또는 감쇠)
		CritMultiplier = 1.0f + GetNormalMultiplier();
	}

	// 3. 최종 피해 배율 계산
	// [최종 피해 배율] = (1 + 증가) × (1 + 증폭) + 추가 증가
	float FinalMultiplier = ((1.0f + GetFinalDamageIncrease()) * (1.0f + GetFinalDamageMultiplier()))
		+ GetFinalDamageAdditional();

	// 4. 최종 피해 계산
	// [최종 피해] = [무기 공격력] × [약점 배율] × [최종 피해 배율] × [1 - 피해 감소]
	float FinalDamage = WeaponDamage * CritMultiplier * FinalMultiplier * (1.0f - TargetDamageReduction);

	UE_LOG(LogTemp, Log, TEXT("[Damage Calc] Weapon: %.1f, Crit: %.2f, Final: %.2f, Target Reduction: %.2f => %.1f"),
		WeaponDamage, CritMultiplier, FinalMultiplier, TargetDamageReduction, FinalDamage);

	return FMath::Max(FinalDamage, 0.0f);
}

// OnRep 함수들
void UGRCombatAttributeSet::OnRep_WeaponBaseDamage(const FGameplayAttributeData& OldWeaponBaseDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, WeaponBaseDamage, OldWeaponBaseDamage);
}

void UGRCombatAttributeSet::OnRep_WeaponDamageIncrease(const FGameplayAttributeData& OldWeaponDamageIncrease)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, WeaponDamageIncrease, OldWeaponDamageIncrease);
}

void UGRCombatAttributeSet::OnRep_WeaponDamageMultiplier(const FGameplayAttributeData& OldWeaponDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, WeaponDamageMultiplier, OldWeaponDamageMultiplier);
}

void UGRCombatAttributeSet::OnRep_BaseCriticalMultiplier(const FGameplayAttributeData& OldBaseCriticalMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, BaseCriticalMultiplier, OldBaseCriticalMultiplier);
}

void UGRCombatAttributeSet::OnRep_CriticalMultiplierAmplify(const FGameplayAttributeData& OldCriticalMultiplierAmplify)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, CriticalMultiplierAmplify, OldCriticalMultiplierAmplify);
}

void UGRCombatAttributeSet::OnRep_CriticalMultiplierAdditional(const FGameplayAttributeData& OldCriticalMultiplierAdditional)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, CriticalMultiplierAdditional, OldCriticalMultiplierAdditional);
}

void UGRCombatAttributeSet::OnRep_NormalMultiplier(const FGameplayAttributeData& OldNormalMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, NormalMultiplier, OldNormalMultiplier);
}

void UGRCombatAttributeSet::OnRep_FinalDamageIncrease(const FGameplayAttributeData& OldFinalDamageIncrease)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, FinalDamageIncrease, OldFinalDamageIncrease);
}

void UGRCombatAttributeSet::OnRep_FinalDamageMultiplier(const FGameplayAttributeData& OldFinalDamageMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, FinalDamageMultiplier, OldFinalDamageMultiplier);
}

void UGRCombatAttributeSet::OnRep_FinalDamageAdditional(const FGameplayAttributeData& OldFinalDamageAdditional)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, FinalDamageAdditional, OldFinalDamageAdditional);
}

void UGRCombatAttributeSet::OnRep_DamageReduction(const FGameplayAttributeData& OldDamageReduction)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, DamageReduction, OldDamageReduction);
}
