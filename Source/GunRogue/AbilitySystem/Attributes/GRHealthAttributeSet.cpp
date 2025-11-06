#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

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

	InitGainDamage(0.0f);
	InitGainHealing(0.0f);
}

void UGRHealthAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGRHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRHealthAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRHealthAttributeSet, MaxShield, COND_None, REPNOTIFY_Always);
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

	// GainDamage 처리
	if (Data.EvaluatedData.Attribute == GetGainDamageAttribute())
	{
		const float LocalDamage = GetGainDamage();
		SetGainDamage(0.0f);

		if (LocalDamage > 0.0f)
		{
			const float RealDealtAmount = ApplyDamageAndReturnRealDealtAmount(LocalDamage);

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
	float RemainDamage = InDamage;
	float DealtAmount = 0.0f;

	// 1. Shield 먼저 소모
	const float OldShield = GetShield();
	if (OldShield > 0.0f)
	{
		if (RemainDamage >= OldShield)
		{
			// Shield 파괴 - 남은 데미지는 무효화 (디자인 명세)
			SetShield(0.0f);
			DealtAmount = OldShield;
			RemainDamage = 0.0f;

			UE_LOG(LogTemp, Warning, TEXT("[Health] Shield Broken! (Absorbed: %.1f)"), DealtAmount);
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
