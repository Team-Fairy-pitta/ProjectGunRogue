#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UGRHealthAttributeSet::UGRHealthAttributeSet()
{
	// 기본값 설정
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitShield(50.0f);
	InitMaxShield(50.0f);

	InitDamage(0.0f);
	InitHealing(0.0f);
}

void UGRHealthAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGRHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRHealthAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRHealthAttributeSet, MaxShield, COND_None, REPNOTIFY_Always);
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

	// Damage 처리
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float LocalDamage = GetDamage();
		SetDamage(0.0f);

		if (LocalDamage > 0.0f)
		{
			float DamageToApply = LocalDamage;

			// 1. Shield 먼저 소모
			const float OldShield = GetShield();
			if (OldShield > 0.0f)
			{
				if (DamageToApply >= OldShield)
				{
					// Shield 파괴 - 남은 데미지는 무효화
					SetShield(0.0f);
					DamageToApply = 0.0f;

					UE_LOG(LogTemp, Warning, TEXT("[Health] Shield Broken!"));
				}
				else
				{
					// Shield만 깎임
					SetShield(OldShield - DamageToApply);
					DamageToApply = 0.0f;

					UE_LOG(LogTemp, Log, TEXT("[Health] Shield: %.1f -> %.1f"),
						OldShield, GetShield());
				}
			}

			// 2. Shield 파괴 시 남은 데미지는 적용 안됨
			if (DamageToApply > 0.0f)
			{
				const float OldHealth = GetHealth();
				const float NewHealth = FMath::Max(OldHealth - DamageToApply, 0.0f);
				SetHealth(NewHealth);

				UE_LOG(LogTemp, Warning, TEXT("[Health] Health: %.1f -> %.1f (Damage: %.1f)"),
					OldHealth, NewHealth, DamageToApply);

				if (NewHealth <= 0.0f)
				{
					UE_LOG(LogTemp, Error, TEXT("[Health] Character Died!"));
				}
			}
		}
	}

	// Healing 처리
	if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		const float LocalHealing = GetHealing();
		SetHealing(0.0f);

		if (LocalHealing > 0.0f)
		{
			const float OldHealth = GetHealth();
			const float NewHealth = FMath::Clamp(OldHealth + LocalHealing, 0.0f, GetMaxHealth());
			SetHealth(NewHealth);

			UE_LOG(LogTemp, Log, TEXT("[Health] Healed: %.1f -> %.1f (+%.1f)"),
				OldHealth, NewHealth, LocalHealing);
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

// ========== Replication 함수들 ==========

void UGRHealthAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRHealthAttributeSet, Health, OldHealth);
}

void UGRHealthAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRHealthAttributeSet, MaxHealth, OldMaxHealth);
}

void UGRHealthAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRHealthAttributeSet, Shield, OldShield);
}

void UGRHealthAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRHealthAttributeSet, MaxShield, OldMaxShield);
}
