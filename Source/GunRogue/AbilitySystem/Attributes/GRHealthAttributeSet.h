#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "GRHealthAttributeSet.generated.h"

// Attribute 접근자 매크로
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_MULTICAST_DELEGATE_SixParams(
	FGRAttributeEvent,
	AActor* /*EffectInstigator*/,
	AActor* /*EffectCauser*/,
	const FGameplayEffectSpec* /*EffectSpec*/,
	float /*EffectMagnitude*/,
	float /*OldValue*/,
	float /*NewValue*/
);

namespace GunRogue::Status
{
	GUNROGUE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Invincible);
}

UCLASS()
class GUNROGUE_API UGRHealthAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UGRHealthAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	// ========== 생존 관련 Attributes ==========

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Shield, Category = "Health")
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, Shield);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxShield, Category = "Health")
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, MaxShield);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ShieldRegenDelay, Category = "Health|Shield")
	FGameplayAttributeData ShieldRegenDelay; // 피해를 받지 않았을 때, 실드 재생이 시작되기 까지 걸리는 시간
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, ShieldRegenDelay);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ShieldRegenInterval, Category = "Shield")
	FGameplayAttributeData ShieldRegenInterval;  // 실드 회복 간격
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, ShieldRegenInterval);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ShieldRegenAmount, Category = "Shield")
	FGameplayAttributeData ShieldRegenAmount; // 실드 회복량
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, ShieldRegenAmount);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ShieldBreakInvincibleDuration, Category = "Health|Shield")
	FGameplayAttributeData ShieldBreakInvincibleDuration; // 실드 파괴 후 x초 무적
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, ShieldBreakInvincibleDuration);

	// ========== Meta Attributes (복제되지 않음) ==========

	UPROPERTY(BlueprintReadOnly, Category = "Health|Meta")
	FGameplayAttributeData GainDamage;
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, GainDamage);

	UPROPERTY(BlueprintReadOnly, Category = "Health|Meta")
	FGameplayAttributeData GainHealing;
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, GainHealing);

	UPROPERTY(BlueprintReadOnly, Category = "Health|Meta")
	FGameplayAttributeData GainShield;
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, GainShield);

	// ========== Delegates ==========

	FGRAttributeEvent OnHealthChanged;
	FGRAttributeEvent OnMaxHealthChanged;
	FGRAttributeEvent OnShieldChanged;
	FGRAttributeEvent OnMaxShieldChanged;

	FGRAttributeEvent OnOutOfHealth;  // 사망
	FGRAttributeEvent OnShieldBroken; // Shield 파괴 (무적 효과용)

protected:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldShield);

	UFUNCTION()
	void OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield);

	UFUNCTION()
	void OnRep_ShieldRegenDelay(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ShieldRegenInterval(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ShieldRegenAmount(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ShieldBreakInvincibleDuration(const FGameplayAttributeData& OldValue);

private:
	// Before 값 저장 (델리게이트용)
	float BeforeHealth;
	float BeforeMaxHealth;
	float BeforeShield;
	float BeforeMaxShield;

	// 실드 재생 타이머
	TMap<TWeakObjectPtr<UAbilitySystemComponent>, FTimerHandle> ShieldRegenTimers;

	void AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
		const FGameplayAttributeData& MaxAttribute,
		float NewMaxValue,
		const FGameplayAttribute& AffectedAttributeProperty) const;

	// 실제 피해량 반환 (흡혈, 통계, 증강용)
	float ApplyDamageAndReturnRealDealtAmount(float InDamage);

	// 실드 시스템 헬퍼 함수
	void HandleShieldBreak(UAbilitySystemComponent* OwningASC);
	void StartShieldRegenTimer(UAbilitySystemComponent* OwningASC, bool bIsInitialRegenDelay = true);
	void ClearShieldRegenTimer(UAbilitySystemComponent* OwningASC);
	void ApplyShieldRegenEffect(UAbilitySystemComponent* OwningASC);
};
