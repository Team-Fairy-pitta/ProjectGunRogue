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


UCLASS()
class GUNROGUE_API UGRHealthAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UGRHealthAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

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
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, Damage);

	// 받는 치유 (임시 계산용)
	UPROPERTY(BlueprintReadOnly, Category = "Health|Meta")
	FGameplayAttributeData Healing;
	ATTRIBUTE_ACCESSORS(UGRHealthAttributeSet, Healing);

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
	void AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
		const FGameplayAttributeData& MaxAttribute,
		float NewMaxValue,
		const FGameplayAttribute& AffectedAttributeProperty) const;

};
