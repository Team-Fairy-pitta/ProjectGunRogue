#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "GRCombatAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


UCLASS()
class GUNROGUE_API UGRCombatAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UGRCombatAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 무기 기본 공격력
	UPROPERTY(BlueprintReadOnly, Category = "Combat|WeaponDamage", ReplicatedUsing = OnRep_WeaponDamage_Base)
	FGameplayAttributeData WeaponDamage_Base;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, WeaponDamage_Base)

	// 무기 공격력 증가 (Flat Addition)
	UPROPERTY(BlueprintReadOnly, Category = "Combat|WeaponDamage", ReplicatedUsing = OnRep_WeaponDamage_Additive)
	FGameplayAttributeData WeaponDamage_Additive;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, WeaponDamage_Additive)

	// 무기 공격력 증폭 (Multiplier)
	UPROPERTY(BlueprintReadOnly, Category = "Combat|WeaponDamage", ReplicatedUsing = OnRep_WeaponDamage_Multiplicative)
	FGameplayAttributeData WeaponDamage_Multiplicative;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, WeaponDamage_Multiplicative)


	// 기본 약점 배율
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Critical", ReplicatedUsing = OnRep_WeaponCriticalMultiplier_Base)
	FGameplayAttributeData WeaponCriticalMultiplier_Base;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, WeaponCriticalMultiplier_Base)

	// 약점 배율 증폭
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Critical", ReplicatedUsing = OnRep_WeaponCriticalMultiplier_Multiplicative)
	FGameplayAttributeData WeaponCriticalMultiplier_Multiplicative;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, WeaponCriticalMultiplier_Multiplicative)

	// 약점 배율 추가 증가
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Critical", ReplicatedUsing = OnRep_WeaponCriticalMultiplier_Bonus)
	FGameplayAttributeData WeaponCriticalMultiplier_Bonus;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, WeaponCriticalMultiplier_Bonus)

	// 일반 배율 (약점이 아닐 때)
		UPROPERTY(BlueprintReadOnly, Category = "Combat|Critical", ReplicatedUsing = OnRep_WeaponNormalMultiplier_Multiplicative)
	FGameplayAttributeData WeaponNormalMultiplier_Multiplicative;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, WeaponNormalMultiplier_Multiplicative)


	// 최종 피해 배율 증가
	UPROPERTY(BlueprintReadOnly, Category = "Combat|FinalDamage", ReplicatedUsing = OnRep_FinalDamage_Additive)
	FGameplayAttributeData FinalDamage_Additive;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, FinalDamage_Additive)

	// 최종 피해 배율 증폭
	UPROPERTY(BlueprintReadOnly, Category = "Combat|FinalDamage", ReplicatedUsing = OnRep_FinalDamage_Multiplicative)
	FGameplayAttributeData FinalDamage_Multiplicative;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, FinalDamage_Multiplicative)

	// 최종 피해 배율 추가 증가
	UPROPERTY(BlueprintReadOnly, Category = "Combat|FinalDamage", ReplicatedUsing = OnRep_FinalDamage_Bonus)
	FGameplayAttributeData FinalDamage_Bonus;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, FinalDamage_Bonus)


	// 피해 감소 배율 (적용 시)
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Defense", ReplicatedUsing = OnRep_DamageReduction)
	FGameplayAttributeData DamageReduction;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, DamageReduction)


	// 약점 공격 여부 (Meta)
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Meta")
	FGameplayAttributeData IsCriticalHit;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, IsCriticalHit)


	// 무기 데미지 계산 (무기 공격력만)
	float CalculateWeaponDamage() const;

	// 약점 배율 계산
	float CalculateCriticalMultiplier(bool bIsCritical) const;

	// 최종 피해 배율 계산
	float CalculateFinalDamageMultiplier() const;

	// 최종 데미지 계산 (모든 요소 통합)
	float CalculateFinalWeaponDamage(bool bIsCritical, float TargetDamageReduction) const;

protected:
	UFUNCTION()
	virtual void OnRep_WeaponDamage_Base(const FGameplayAttributeData& OldWeaponDamageBase);

	UFUNCTION()
	virtual void OnRep_WeaponDamage_Additive(const FGameplayAttributeData& OldWeaponDamageAdditive);

	UFUNCTION()
	virtual void OnRep_WeaponDamage_Multiplicative(const FGameplayAttributeData& OldWeaponDamageMultiplicative);

	UFUNCTION()
	virtual void OnRep_WeaponCriticalMultiplier_Base(const FGameplayAttributeData& OldWeaponCriticalMultiplierBase);

	UFUNCTION()
	virtual void OnRep_WeaponCriticalMultiplier_Multiplicative(const FGameplayAttributeData& OldWeaponCriticalMultiplierMultiplicative);

	UFUNCTION()
	virtual void OnRep_WeaponCriticalMultiplier_Bonus(const FGameplayAttributeData& OldWeaponCriticalMultiplierBonus);

	UFUNCTION()
	virtual void OnRep_WeaponNormalMultiplier_Multiplicative(const FGameplayAttributeData& OldWeaponNormalMultiplierMultiplicative);

	UFUNCTION()
	virtual void OnRep_FinalDamage_Additive(const FGameplayAttributeData& OldFinalDamageAdditive);

	UFUNCTION()
	virtual void OnRep_FinalDamage_Multiplicative(const FGameplayAttributeData& OldFinalDamageMultiplicative);

	UFUNCTION()
	virtual void OnRep_FinalDamage_Bonus(const FGameplayAttributeData& OldFinalDamageBonus);

	UFUNCTION()
	virtual void OnRep_DamageReduction(const FGameplayAttributeData& OldDamageReduction);
};
