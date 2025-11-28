#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

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

	FireRate = 2.0f;
	Accuracy = 1.0f;
	Recoil = 1.0f;
	SpreadRecoveryRate = 0.5f;
	MaxSpread = 10.0f;
	SpreadIncreasePerShot = 2.0f;
	CurrentSpread = 0.0f;
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

	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, FireRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, Accuracy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, Recoil, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, SpreadRecoveryRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, MaxSpread, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, SpreadIncreasePerShot, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRCombatAttributeSet, CurrentSpread, COND_None, REPNOTIFY_Always);
}

void UGRCombatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// CurrentSpread 범위 제한
	if (Attribute == GetCurrentSpreadAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxSpread());
	}
	else if (Attribute == GetSpreadRecoveryRateAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
}

void UGRCombatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetCurrentSpreadAttribute())
	{
		SetCurrentSpread(FMath::Clamp(GetCurrentSpread(), 0.0f, GetMaxSpread()));
	}
}

void UGRCombatAttributeSet::IncreaseSpread(UAbilitySystemComponent* OwningASC)
{
	if (!OwningASC)
	{
		return;
	}
	if (OwningASC->GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	const float CurrentSpreadValue = GetCurrentSpread();
	const float IncreaseAmount = GetSpreadIncreasePerShot();
	const float MaxSpreadValue = GetMaxSpread();

	const float NewSpread = FMath::Min(CurrentSpreadValue + IncreaseAmount, MaxSpreadValue);
	SetCurrentSpread(NewSpread);

	UE_LOG(LogTemp, Log, TEXT("[Spread] Increased: %.2f -> %.2f (+%.2f) [%s]"),
		CurrentSpreadValue, NewSpread, IncreaseAmount,
		OwningASC->GetOwnerRole() == ROLE_Authority ? TEXT("Server") : TEXT("Client"));

	ClearSpreadRecoveryTimer(OwningASC);
	StartSpreadRecoveryTimer(OwningASC, true);
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

void UGRCombatAttributeSet::StartSpreadRecoveryTimer(UAbilitySystemComponent* OwningASC, bool bIsInitialDelay)
{
	if (!OwningASC)
	{
		return;
	}

	if (OwningASC->GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	AActor* OwnerActor = OwningASC->GetOwnerActor();
	if (!OwnerActor || !OwnerActor->GetWorld())
	{
		return;
	}

	const float CurrentSpreadValue = GetCurrentSpread();
	const float MaxSpreadValue = GetMaxSpread();

	// 이미 0이면 타이머 필요 없음
	if (CurrentSpreadValue <= 0.0f)
	{
		SpreadRecoveryTimers.Remove(OwningASC);
		return;
	}

	// 기존 타이머 취소
	ClearSpreadRecoveryTimer(OwningASC);

	FTimerHandle& RecoveryTimerHandle = SpreadRecoveryTimers.FindOrAdd(OwningASC);
	FTimerDelegate RecoveryDelegate;

	TWeakObjectPtr<UGRCombatAttributeSet> WeakThis(this);
	TWeakObjectPtr<UAbilitySystemComponent> WeakOwningASC(OwningASC);

	RecoveryDelegate.BindLambda([WeakThis, WeakOwningASC]()
		{
			if (!WeakThis.IsValid() || !WeakOwningASC.IsValid())
			{
				return;
			}

			if (WeakOwningASC->GetOwnerRole() != ROLE_Authority)
			{
				return;
			}

			ThisClass* This = WeakThis.Get();
			UAbilitySystemComponent* ASC = WeakOwningASC.Get();

			const float CurrentSpread = This->GetCurrentSpread();

			// 완전히 회복되었으면 타이머 제거
			if (CurrentSpread <= 0.0f)
			{
				This->SpreadRecoveryTimers.Remove(ASC);
				UE_LOG(LogTemp, Log, TEXT("[Spread] Recovery completed"));
				return;
			}

			// 회복 적용
			This->ApplySpreadRecovery(ASC);

			// 아직 남아있으면 다음 틱 예약
			if (This->GetCurrentSpread() > 0.0f)
			{
				This->StartSpreadRecoveryTimer(ASC, false);  // 이후는 Interval
			}
			else
			{
				This->SpreadRecoveryTimers.Remove(ASC);
				UE_LOG(LogTemp, Log, TEXT("[Spread] Recovery completed"));
			}
		});

	// 첫 실행은 SPREAD_RECOVERY_DELAY (0초), 이후는 SPREAD_RECOVERY_INTERVAL (0.1초)
	const float TimerDelay = bIsInitialDelay ? SPREAD_RECOVERY_DELAY : SPREAD_RECOVERY_INTERVAL;

	OwnerActor->GetWorld()->GetTimerManager().SetTimer(
		RecoveryTimerHandle,
		RecoveryDelegate,
		TimerDelay,
		false  // 반복 안 함 (수동으로 재예약)
	);

	if (bIsInitialDelay)
	{
		UE_LOG(LogTemp, Log, TEXT("[Spread] Recovery starting immediately"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[Spread] Next recovery tick in %.1f seconds"), TimerDelay);
	}
}

void UGRCombatAttributeSet::ClearSpreadRecoveryTimer(UAbilitySystemComponent* OwningASC)
{
	if (!OwningASC)
	{
		return;
	}

	FTimerHandle* TimerHandle = SpreadRecoveryTimers.Find(OwningASC);
	if (!TimerHandle)
	{
		return;
	}

	// Invalid 타이머도 제거
	if (!TimerHandle->IsValid())
	{
		SpreadRecoveryTimers.Remove(OwningASC);
		return;
	}

	AActor* OwnerActor = OwningASC->GetOwnerActor();
	if (!OwnerActor || !OwnerActor->GetWorld())
	{
		return;
	}

	OwnerActor->GetWorld()->GetTimerManager().ClearTimer(*TimerHandle);
	SpreadRecoveryTimers.Remove(OwningASC);
	UE_LOG(LogTemp, Log, TEXT("[Spread] Recovery timer cleared"));
}

void UGRCombatAttributeSet::ApplySpreadRecovery(UAbilitySystemComponent* OwningASC)
{
	if (!OwningASC || OwningASC->GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	const float RecoveryRate = GetSpreadRecoveryRate();
	const float CurrentSpreadValue = GetCurrentSpread();

	// RecoveryRate는 초당 회복량이므로 SPREAD_RECOVERY_INTERVAL을 곱함
	const float RecoveryAmount = RecoveryRate * SPREAD_RECOVERY_INTERVAL;
	const float NewSpread = FMath::Max(0.0f, CurrentSpreadValue - RecoveryAmount);

	if (CurrentSpreadValue <= 0.0f)
	{
		return;
	}

	SetCurrentSpread(NewSpread);

	UE_LOG(LogTemp, Log, TEXT("[Spread] Recovered: %.2f -> %.2f (-%.2f)"),
		CurrentSpreadValue, NewSpread, RecoveryAmount);
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

void UGRCombatAttributeSet::OnRep_FireRate(const FGameplayAttributeData& OldFireRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, FireRate, OldFireRate);
}

void UGRCombatAttributeSet::OnRep_Accuracy(const FGameplayAttributeData& OldAccuracy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, Accuracy, OldAccuracy);
}

void UGRCombatAttributeSet::OnRep_Recoil(const FGameplayAttributeData& OldRecoil)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, Recoil, OldRecoil);
}

void UGRCombatAttributeSet::OnRep_SpreadRecoveryRate(const FGameplayAttributeData& OldSpreadRecoveryRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, SpreadRecoveryRate, OldSpreadRecoveryRate);
}

void UGRCombatAttributeSet::OnRep_MaxSpread(const FGameplayAttributeData& OldMaxSpread)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, MaxSpread, OldMaxSpread);
}

void UGRCombatAttributeSet::OnRep_SpreadIncreasePerShot(const FGameplayAttributeData& OldSpreadIncreasePerShot)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, SpreadIncreasePerShot, OldSpreadIncreasePerShot);
}

void UGRCombatAttributeSet::OnRep_CurrentSpread(const FGameplayAttributeData& OldCurrentSpread)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRCombatAttributeSet, CurrentSpread, OldCurrentSpread);
}
