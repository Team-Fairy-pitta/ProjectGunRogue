#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameplayTagContainer.h"
#include "TimerManager.h"
#include "Engine/World.h"

namespace GunRogue::Status
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Invincible, "Status.Invincible", "");
}

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

	InitShieldRegenDelay(5.0f);
	InitShieldRegenInterval(1.0f);
	InitShieldRegenAmount(5.0f);
	InitShieldBreakInvincibleDuration(0.5f);

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
	DOREPLIFETIME_CONDITION_NOTIFY(UGRHealthAttributeSet, ShieldRegenInterval, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRHealthAttributeSet, ShieldRegenAmount, COND_None, REPNOTIFY_Always);
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
		UAbilitySystemComponent* OwningASC = GetOwningAbilitySystemComponent();
		if (!OwningASC)
		{
			return false;
		}

		if (OwningASC->HasMatchingGameplayTag(GunRogue::Status::Status_Invincible))
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

	// 음수 방어 로직
	// 생명력 관련 (최소값 0)
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);  // MaxHealth는 최소 1
	}
	// 실드 관련 (최소값 0)
	else if (Attribute == GetShieldAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	else if (Attribute == GetMaxShieldAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);  // MaxShield는 0 가능
	}
	// 실드 회복 관련
	else if (Attribute == GetShieldRegenDelayAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);  // 음수 방지
	}
	else if (Attribute == GetShieldRegenIntervalAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.1f);  // 최소 0.1초 (너무 빠른 회복 방지)
	}
	else if (Attribute == GetShieldRegenAmountAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);  // 음수 방지
	}
	else if (Attribute == GetShieldBreakInvincibleDurationAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);  // 음수 방지
	}
}

void UGRHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();
	UAbilitySystemComponent* OwningASC = GetOwningAbilitySystemComponent();

	// GainDamage 처리
	if (Data.EvaluatedData.Attribute == GetGainDamageAttribute())
	{
		const float LocalDamage = GetGainDamage();
		SetGainDamage(0.0f);

		if (LocalDamage > 0.0f)
		{
			const float RealDealtAmount = ApplyDamageAndReturnRealDealtAmount(LocalDamage);
			const bool bShieldWasBroken = (BeforeShield > 0.0f && GetShield() <= 0.0f);

			if (OwningASC && OwningASC->GetOwnerRole() == ROLE_Authority)
			{
				// 실드가 파괴되었을 때 무적 부여
				if (bShieldWasBroken)
				{
					HandleShieldBreak(OwningASC);
				}

				// 공격 받을 시, 실드 회복 타이머 리셋
				if (RealDealtAmount > 0.0)
				{
					ClearShieldRegenTimer(OwningASC);
					StartShieldRegenTimer(OwningASC);
				}

				if (GetShield() >= GetMaxShield())
				{
					ClearShieldRegenTimer(OwningASC);
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

void UGRHealthAttributeSet::HandleShieldBreak(UAbilitySystemComponent* OwningASC)
{
	if (!OwningASC)
	{
		return;
	}

	// 무적 태그 부여
	FGameplayTagContainer InvincibleTags;
	InvincibleTags.AddTag(GunRogue::Status::Status_Invincible);
	OwningASC->AddLooseGameplayTags(InvincibleTags);

	AActor* OwnerActor = OwningASC->GetOwnerActor();
	if (!OwnerActor || !OwnerActor->GetWorld())
	{
		return;
	}

	// 무적 해제 타이머
	FTimerHandle InvincibleTimerHandle;
	FTimerDelegate InvincibleDelegate;

	TWeakObjectPtr<UAbilitySystemComponent> WeakOwningASC(OwningASC);

	InvincibleDelegate.BindLambda([WeakOwningASC, InvincibleTags]()
		{
			if (WeakOwningASC.IsValid())
			{
				WeakOwningASC->RemoveLooseGameplayTags(InvincibleTags);
				UE_LOG(LogTemp, Log, TEXT("[Shield] Invincible ended"));
			}
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
	ClearShieldRegenTimer(OwningASC);
}

void UGRHealthAttributeSet::StartShieldRegenTimer(UAbilitySystemComponent* OwningASC, bool bIsInitialRegenDelay)
{
	if (!OwningASC)
	{
		return;
	}

	// 서버 체크
	if (OwningASC->GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	AActor* OwnerActor = OwningASC->GetOwnerActor();
	if (!OwnerActor || !OwnerActor->GetWorld())
	{
		return;
	}

	const float CurrentShield = GetShield();
	const float MaxShieldValue = GetMaxShield();

	if (CurrentShield >= MaxShieldValue)
	{
		ShieldRegenTimers.Remove(OwningASC);
		return;
	}

	// 기존 타이머 취소
	ClearShieldRegenTimer(OwningASC);

	FTimerHandle& RegenTimerHandle = ShieldRegenTimers.FindOrAdd(OwningASC);
	FTimerDelegate RegenDelegate;

	TWeakObjectPtr<ThisClass> WeakThis(this);
	TWeakObjectPtr<UAbilitySystemComponent> WeakOwningASC(OwningASC);

	RegenDelegate.BindLambda([WeakThis, WeakOwningASC]()
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

			const float CurrentShield = This->GetShield();
			const float MaxShieldValue = This->GetMaxShield();

			if (CurrentShield >= MaxShieldValue)
			{
				This->ShieldRegenTimers.Remove(ASC);
				UE_LOG(LogTemp, Log, TEXT("[Shield] Regen completed"));
				return;
			}

			This->ApplyShieldRegenEffect(ASC);

			if (This->GetShield() < MaxShieldValue)
			{
				This->StartShieldRegenTimer(ASC, false);
			}
			else
			{
				This->ShieldRegenTimers.Remove(ASC);
				UE_LOG(LogTemp, Log, TEXT("[Shield] Regen completed"));
			}
		});

	// 첫 실행은 ShieldRegenDelay, 이후는 ShieldRegenInterval
	const float TimerDelay = bIsInitialRegenDelay ? GetShieldRegenDelay() : GetShieldRegenInterval();

	OwnerActor->GetWorld()->GetTimerManager().SetTimer(
		RegenTimerHandle,
		RegenDelegate,
		TimerDelay,
		false
	);

	if (bIsInitialRegenDelay)
	{
		UE_LOG(LogTemp, Log, TEXT("[Shield] Regen will start in %.1f seconds"), TimerDelay);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[Shield] Next regen tick in %.1f seconds"), TimerDelay);
	}

}

void UGRHealthAttributeSet::ClearShieldRegenTimer(UAbilitySystemComponent* OwningASC)
{
	if (!OwningASC)
	{
		return;
	}

	FTimerHandle* TimerHandle = ShieldRegenTimers.Find(OwningASC);
	if (!TimerHandle)
	{
		return;
	}

	// Invalid 타이머도 제거
	if (!TimerHandle->IsValid())
	{
		ShieldRegenTimers.Remove(OwningASC);
		return;
	}

	AActor* OwnerActor = OwningASC->GetOwnerActor();
	if (!OwnerActor || !OwnerActor->GetWorld())
	{
		return;
	}

	OwnerActor->GetWorld()->GetTimerManager().ClearTimer(*TimerHandle);
	ShieldRegenTimers.Remove(OwningASC);

	UE_LOG(LogTemp, Log, TEXT("[Shield] Regen timer cleared"));
}

void UGRHealthAttributeSet::ApplyShieldRegenEffect(UAbilitySystemComponent* OwningASC)
{
	if (!OwningASC || OwningASC->GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	const float CurrentShield = GetShield();
	const float MaxShieldValue = GetMaxShield();
	const float RegenAmount = GetShieldRegenAmount();

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

// OnRep 함수들 - 클라이언트에서도 델리게이트 브로드캐스트
void UGRHealthAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRHealthAttributeSet, Health, OldHealth);

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

	float OldValue = OldMaxHealth.GetCurrentValue();
	float NewValue = GetMaxHealth();
	OnMaxHealthChanged.Broadcast(nullptr, nullptr, nullptr, NewValue - OldValue, OldValue, NewValue);
}

void UGRHealthAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRHealthAttributeSet, Shield, OldShield);

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

	float OldValue = OldMaxShield.GetCurrentValue();
	float NewValue = GetMaxShield();
	OnMaxShieldChanged.Broadcast(nullptr, nullptr, nullptr, NewValue - OldValue, OldValue, NewValue);
}

void UGRHealthAttributeSet::OnRep_ShieldRegenDelay(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRHealthAttributeSet, ShieldRegenDelay, OldValue);
}

void UGRHealthAttributeSet::OnRep_ShieldRegenInterval(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRHealthAttributeSet, ShieldRegenInterval, OldValue);
}

void UGRHealthAttributeSet::OnRep_ShieldRegenAmount(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRHealthAttributeSet, ShieldRegenAmount, OldValue);
}

void UGRHealthAttributeSet::OnRep_ShieldBreakInvincibleDuration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRHealthAttributeSet, ShieldBreakInvincibleDuration, OldValue);
}
