#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameplayTagContainer.h"
#include "TimerManager.h"
#include "Engine/World.h"

UGRHealthAttributeSet::UGRHealthAttributeSet()
	: BeforeHealth(0.0f)
	, BeforeMaxHealth(0.0f)
	, BeforeShield(0.0f)
	, BeforeMaxShield(0.0f)
{
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitShield(50.0f);
	InitMaxShield(50.0f);

	// 실드 설정 기본값
	InitShieldRegenDelay(3.0f);     // 3초 후 회복 시작
	InitShieldRegenRate(10.0f);      // 초당 10 회복
	InitShieldBreakInvincibleDuration(0.5f);  // 0.5초 무적

	InitGainDamage(0.0f);
	InitGainHealing(0.0f);
	InitGainShield(0.0f);
}

void UGRHealthAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGRHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRHealthAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRHealthAttributeSet, MaxShield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRHealthAttributeSet, ShieldRegenDelay, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRHealthAttributeSet, ShieldRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRHealthAttributeSet, ShieldBreakInvincibleDuration, COND_None, REPNOTIFY_Always);
}

bool UGRHealthAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	// Before 값 저장 (델리게이트에서 사용)
	BeforeHealth = GetHealth();
	BeforeMaxHealth = GetMaxHealth();
	BeforeShield = GetShield();
	BeforeMaxShield = GetMaxShield();

	// 무적 상태 체크 (GainDamage일 때만)
	if (Data.EvaluatedData.Attribute == GetGainDamageAttribute())
	{
		UAbilitySystemComponent* TargetASC = GetOwningAbilitySystemComponent();
		if (!TargetASC)
		{
			return false;
		}

		if (TargetASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Status.Invincible"))))
		{
			UE_LOG(LogTemp, Warning, TEXT("[Damage] Target is invincible - Damage blocked"));
			return false;  // 피해 차단
		}
	}

	return true;
}

void UGRHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxShieldAttribute())
	{
		AdjustAttributeForMaxChange(Shield, MaxShield, NewValue, GetShieldAttribute());
	}
}

void UGRHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();
	UAbilitySystemComponent* TargetASC = GetOwningAbilitySystemComponent();

	// GainDamage 처리
	if (Data.EvaluatedData.Attribute == GetGainDamageAttribute())
	{
		const float LocalDamage = GetGainDamage();
		SetGainDamage(0.0f);

		if (LocalDamage > 0.0f)
		{
			const float RealDealtAmount = ApplyDamageAndReturnRealDealtAmount(LocalDamage);
			const bool bShieldWasBroken = (BeforeShield > 0.0f && GetShield() <= 0.0f);

			// 실드가 파괴되었을 때 무적 부여
			if (bShieldWasBroken && TargetASC && TargetASC->GetOwnerRole() == ROLE_Authority)
			{
				HandleShieldBreak(TargetASC, Instigator, Causer, &(Data.EffectSpec));
			}

			// 실드가 피격되면 회복 타이머 리셋
			if (RealDealtAmount > 0.0f && TargetASC && TargetASC->GetOwnerRole() == ROLE_Authority)
			{
				// 실드가 최대치가 아니면 타이머 리셋
				if (GetShield() < GetMaxShield())
				{
					ClearShieldRegenTimer(TargetASC);
					StartShieldRegenTimer(TargetASC);
				}
			}

			// TODO: 여기서 RealDealtAmount를 사용해 흡혈, 궁극기 게이지 등 구현 가능
			// 예: GainUltimateGauge(RealDealtAmount);
		}
	}

	// GainHealing 처리
	if (Data.EvaluatedData.Attribute == GetGainHealingAttribute())
	{
		const float LocalHealing = GetGainHealing();
		SetGainHealing(0.0f);

		if (LocalHealing > 0.0f)
		{
			const float NewHealth = FMath::Clamp(GetHealth() + LocalHealing, 0.0f, GetMaxHealth());
			SetHealth(NewHealth);
		}
	}

	// GainShield 처리
	if (Data.EvaluatedData.Attribute == GetGainShieldAttribute())
	{
		const float LocalShieldGain = GetGainShield();
		SetGainShield(0.0f);

		if (LocalShieldGain > 0.0f)
		{
			const float OldShield = GetShield();
			const float NewShield = FMath::Clamp(OldShield + LocalShieldGain, 0.0f, GetMaxShield());
			SetShield(NewShield);

			UE_LOG(LogTemp, Log, TEXT("[Health] Shield Restored: %.1f -> %.1f (+%.1f)"),
				OldShield, NewShield, LocalShieldGain);
		}
	}

	// Health/Shield Clamping
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetShieldAttribute())
	{
		SetShield(FMath::Clamp(GetShield(), 0.0f, GetMaxShield()));
	}

	// 델리게이트 브로드캐스트
	if (GetHealth() != BeforeHealth)
	{
		OnHealthChanged.Broadcast(Instigator, Causer, &(Data.EffectSpec),
			Data.EvaluatedData.Magnitude, BeforeHealth, GetHealth());

		if (GetHealth() <= 0.0f && BeforeHealth > 0.0f)
		{
			OnOutOfHealth.Broadcast(Instigator, Causer, &(Data.EffectSpec),
				Data.EvaluatedData.Magnitude, BeforeHealth, GetHealth());
		}
	}

	if (GetShield() != BeforeShield)
	{
		OnShieldChanged.Broadcast(Instigator, Causer, &(Data.EffectSpec),
			Data.EvaluatedData.Magnitude, BeforeShield, GetShield());

		// Shield 파괴 이벤트 (무적 효과용)
		if (GetShield() <= 0.0f && BeforeShield > 0.0f)
		{
			OnShieldBroken.Broadcast(Instigator, Causer, &(Data.EffectSpec),
				Data.EvaluatedData.Magnitude, BeforeShield, GetShield());
		}
	}

	if (GetMaxHealth() != BeforeMaxHealth)
	{
		OnMaxHealthChanged.Broadcast(Instigator, Causer, &(Data.EffectSpec),
			Data.EvaluatedData.Magnitude, BeforeMaxHealth, GetMaxHealth());
	}

	if (GetMaxShield() != BeforeMaxShield)
	{
		OnMaxShieldChanged.Broadcast(Instigator, Causer, &(Data.EffectSpec),
			Data.EvaluatedData.Magnitude, BeforeMaxShield, GetMaxShield());
	}
}

float UGRHealthAttributeSet::ApplyDamageAndReturnRealDealtAmount(float InDamage)
{
	if (InDamage <= 0.0f)
	{
		return 0.0f;
	}

	float RemainDamage = InDamage;
	float DealtAmount = 0.0f;

	// 1. Shield 먼저 소모
	const float OldShield = GetShield();
	if (OldShield > 0.0f)
	{
		if (RemainDamage >= OldShield)
		{
			// Shield 파괴 - 남은 데미지는 무효화
			SetShield(0.0f);
			DealtAmount = OldShield;
			UE_LOG(LogTemp, Warning, TEXT("[Health] Shield Broken! (Absorbed: %.1f)"), RemainDamage - DealtAmount);

			RemainDamage = 0.0f;
		}
		else
		{
			// Shield만 깎임
			SetShield(OldShield - RemainDamage);
			DealtAmount = RemainDamage;
			RemainDamage = 0.0f;
		}
	}

	// 2. 남은 데미지를 Health에 적용 (Shield 파괴 시에는 실행 안됨)
	if (RemainDamage > 0.0f)
	{
		const float OldHealth = GetHealth();
		const float ReducedHealth = FMath::Min(OldHealth, RemainDamage);
		SetHealth(OldHealth - ReducedHealth);
		DealtAmount += ReducedHealth;

		UE_LOG(LogTemp, Warning, TEXT("[Health] Health: %.1f -> %.1f"), OldHealth, GetHealth());
	}

	return DealtAmount;
}

void UGRHealthAttributeSet::HandleShieldBreak(UAbilitySystemComponent* TargetASC, AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec)
{
	if (!TargetASC)
	{
		return;
	}

	// 무적 태그 부여
	FGameplayTagContainer InvincibleTags;
	InvincibleTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Status.Invincible")));
	TargetASC->AddLooseGameplayTags(InvincibleTags);

	AActor* OwnerActor = TargetASC->GetOwnerActor();
	if (!OwnerActor || !OwnerActor->GetWorld())
	{
		return;
	}

	// 무적 해제 타이머
	FTimerHandle InvincibleTimerHandle;
	FTimerDelegate InvincibleDelegate;

	InvincibleDelegate.BindLambda([TargetASC, InvincibleTags]()
		{
			if (!TargetASC)
			{
				return;
			}

			TargetASC->RemoveLooseGameplayTags(InvincibleTags);
			UE_LOG(LogTemp, Log, TEXT("[Shield] Invincible ended"));
		});

	OwnerActor->GetWorld()->GetTimerManager().SetTimer(
		InvincibleTimerHandle,
		InvincibleDelegate,
		GetShieldBreakInvincibleDuration(),
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("[Shield] Invincible applied for %.2f seconds"),
		GetShieldBreakInvincibleDuration());

	// 실드 회복 타이머 제거
	ClearShieldRegenTimer(TargetASC);
}

void UGRHealthAttributeSet::StartShieldRegenTimer(UAbilitySystemComponent* TargetASC, bool bUseDelay)
{
	if (!TargetASC)
	{
		return;
	}

	// 서버 체크
	if (TargetASC->GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	AActor* OwnerActor = TargetASC->GetOwnerActor();
	if (!OwnerActor || !OwnerActor->GetWorld())
	{
		return;
	}

	float RegenDelay = GetShieldRegenDelay();
	if (RegenDelay <= 0.0f)
	{
		RegenDelay = 1.0f; // 최소 1초 딜레이
	}

	// 기존 타이머 취소
	ClearShieldRegenTimer(TargetASC);

	FTimerHandle& RegenTimerHandle = ShieldRegenTimers.FindOrAdd(TargetASC);
	FTimerDelegate RegenDelegate;

	RegenDelegate.BindLambda([this, TargetASC]()
		{
			if (!TargetASC || TargetASC->GetOwnerRole() != ROLE_Authority)
			{
				return;
			}

			const float CurrentShield = GetShield();
			const float MaxShieldValue = GetMaxShield();

			if (CurrentShield >= MaxShieldValue)
			{
				ShieldRegenTimers.Remove(TargetASC);
				return;
			}

			ApplyShieldRegenEffect(TargetASC);

			// 다음 회복 예약
			if (GetShield() < MaxShieldValue)
			{
				StartShieldRegenTimer(TargetASC, false);
			}
			else
			{
				ShieldRegenTimers.Remove(TargetASC);
			}
		});

	const float TimerDelay = bUseDelay ? GetShieldRegenDelay() : 1.0f;

	OwnerActor->GetWorld()->GetTimerManager().SetTimer(
		RegenTimerHandle,
		RegenDelegate,
		TimerDelay,
		false
	);

	UE_LOG(LogTemp, Log, TEXT("[Shield] Regen timer started (Server): %.1f seconds"), RegenDelay);

}

void UGRHealthAttributeSet::ClearShieldRegenTimer(UAbilitySystemComponent* TargetASC)
{
	if (!TargetASC)
	{
		return;
	}

	FTimerHandle* TimerHandle = ShieldRegenTimers.Find(TargetASC);
	if (!TimerHandle || !TimerHandle->IsValid())
	{
		return;
	}

	AActor* OwnerActor = TargetASC->GetOwnerActor();
	if (!OwnerActor || !OwnerActor->GetWorld())
	{
		return;
	}

	OwnerActor->GetWorld()->GetTimerManager().ClearTimer(*TimerHandle);
	ShieldRegenTimers.Remove(TargetASC);

	UE_LOG(LogTemp, Log, TEXT("[Shield] Regen timer cleared"));
}

void UGRHealthAttributeSet::ApplyShieldRegenEffect(UAbilitySystemComponent* TargetASC)
{
	if (!TargetASC || TargetASC->GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	const float CurrentShield = GetShield();
	const float MaxShieldValue = GetMaxShield();
	const float RegenAmount = GetShieldRegenRate();

	if (CurrentShield >= MaxShieldValue)
	{
		return;
	}

	const float NewShield = FMath::Min(CurrentShield + RegenAmount, MaxShieldValue);
	SetShield(NewShield);

	UE_LOG(LogTemp, Log, TEXT("[Shield] Regenerated: %.1f -> %.1f (+%.1f)"),
		CurrentShield, NewShield, RegenAmount);
}

void UGRHealthAttributeSet::AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
	const FGameplayAttributeData& MaxAttribute, float NewMaxValue,
	const FGameplayAttribute& AffectedAttributeProperty) const
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && ASC)
	{
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		const float NewDelta = (CurrentMaxValue > 0.0f)
			? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue
			: NewMaxValue;

		ASC->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

// OnRep 함수들
void UGRHealthAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRHealthAttributeSet, Health, OldHealth);

	// 클라이언트에서도 델리게이트 브로드캐스트
	float OldValue = OldHealth.GetCurrentValue();
	float NewValue = GetHealth();
	OnHealthChanged.Broadcast(nullptr, nullptr, nullptr, NewValue - OldValue, OldValue, NewValue);

	if (NewValue <= 0.0f && OldValue > 0.0f)
	{
		OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, NewValue - OldValue, OldValue, NewValue);
	}
}

void UGRHealthAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRHealthAttributeSet, MaxHealth, OldMaxHealth);

	// 클라이언트에서도 델리게이트 브로드캐스트
	float OldValue = OldMaxHealth.GetCurrentValue();
	float NewValue = GetMaxHealth();
	OnMaxHealthChanged.Broadcast(nullptr, nullptr, nullptr, NewValue - OldValue, OldValue, NewValue);
}

void UGRHealthAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRHealthAttributeSet, Shield, OldShield);

	// 클라이언트에서도 델리게이트 브로드캐스트
	float OldValue = OldShield.GetCurrentValue();
	float NewValue = GetShield();
	OnShieldChanged.Broadcast(nullptr, nullptr, nullptr, NewValue - OldValue, OldValue, NewValue);

	if (NewValue <= 0.0f && OldValue > 0.0f)
	{
		OnShieldBroken.Broadcast(nullptr, nullptr, nullptr, NewValue - OldValue, OldValue, NewValue);
	}
}

void UGRHealthAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRHealthAttributeSet, MaxShield, OldMaxShield);

	// 클라이언트에서도 델리게이트 브로드캐스트
	float OldValue = OldMaxShield.GetCurrentValue();
	float NewValue = GetMaxShield();
	OnMaxShieldChanged.Broadcast(nullptr, nullptr, nullptr, NewValue - OldValue, OldValue, NewValue);
}

void UGRHealthAttributeSet::OnRep_ShieldRegenDelay(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRHealthAttributeSet, ShieldRegenDelay, OldValue);
}

void UGRHealthAttributeSet::OnRep_ShieldRegenRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRHealthAttributeSet, ShieldRegenRate, OldValue);
}

void UGRHealthAttributeSet::OnRep_ShieldBreakInvincibleDuration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRHealthAttributeSet, ShieldBreakInvincibleDuration, OldValue);
}
