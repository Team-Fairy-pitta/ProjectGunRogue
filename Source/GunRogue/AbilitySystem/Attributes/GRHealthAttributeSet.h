#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
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

	// 현재 생명력
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, Health);

	// 최대 생명력
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, MaxHealth);

	// 현재 보호막
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Shield, Category = "Health")
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, Shield);

	// 최대 보호막
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxShield, Category = "Health")
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, MaxShield);

	// ========== Meta Attributes (복제되지 않음) ==========

	// 받는 피해 (임시 계산용)
	UPROPERTY(BlueprintReadOnly, Category = "Health|Meta")
	FGameplayAttributeData GainDamage;
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, GainDamage);

	// 받는 치유 (임시 계산용)
	UPROPERTY(BlueprintReadOnly, Category = "Health|Meta")
	FGameplayAttributeData GainHealing;
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, GainHealing);

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

private:
	// Before 값 저장 (델리게이트용)
	float BeforeHealth;
	float BeforeMaxHealth;
	float BeforeShield;
	float BeforeMaxShield;

	void AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
		const FGameplayAttributeData& MaxAttribute,
		float NewMaxValue,
		const FGameplayAttribute& AffectedAttributeProperty) const;

	// 실제 피해량 반환 (흡혈, 통계, 증강용)
	float ApplyDamageAndReturnRealDealtAmount(float InDamage);
};
