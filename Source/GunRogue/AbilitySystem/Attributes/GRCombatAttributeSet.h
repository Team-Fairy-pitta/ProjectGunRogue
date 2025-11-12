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
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Weapon", ReplicatedUsing = OnRep_WeaponBaseDamage)
	FGameplayAttributeData WeaponBaseDamage;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, WeaponBaseDamage)

	// 무기 공격력 증가 (Flat Addition)
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Weapon", ReplicatedUsing = OnRep_WeaponDamageIncrease)
	FGameplayAttributeData WeaponDamageIncrease;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, WeaponDamageIncrease)

	// 무기 공격력 증폭 (Multiplier)
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Weapon", ReplicatedUsing = OnRep_WeaponDamageMultiplier)
	FGameplayAttributeData WeaponDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, WeaponDamageMultiplier)


	// 기본 약점 배율
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Critical", ReplicatedUsing = OnRep_BaseCriticalMultiplier)
	FGameplayAttributeData BaseCriticalMultiplier;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, BaseCriticalMultiplier)

	// 약점 배율 증폭
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Critical", ReplicatedUsing = OnRep_CriticalMultiplierAmplify)
	FGameplayAttributeData CriticalMultiplierAmplify;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, CriticalMultiplierAmplify)

	// 약점 배율 추가 증가
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Critical", ReplicatedUsing = OnRep_CriticalMultiplierAdditional)
	FGameplayAttributeData CriticalMultiplierAdditional;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, CriticalMultiplierAdditional)

	// 일반 배율 (약점이 아닐 때)
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Critical", ReplicatedUsing = OnRep_NormalMultiplier)
	FGameplayAttributeData NormalMultiplier;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, NormalMultiplier)


	// 최종 피해 배율 증가
	UPROPERTY(BlueprintReadOnly, Category = "Combat|FinalDamage", ReplicatedUsing = OnRep_FinalDamageIncrease)
	FGameplayAttributeData FinalDamageIncrease;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, FinalDamageIncrease)

	// 최종 피해 배율 증폭
	UPROPERTY(BlueprintReadOnly, Category = "Combat|FinalDamage", ReplicatedUsing = OnRep_FinalDamageMultiplier)
	FGameplayAttributeData FinalDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, FinalDamageMultiplier)

	// 최종 피해 배율 추가 증가
	UPROPERTY(BlueprintReadOnly, Category = "Combat|FinalDamage", ReplicatedUsing = OnRep_FinalDamageAdditional)
	FGameplayAttributeData FinalDamageAdditional;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, FinalDamageAdditional)


	// 피해 감소 배율 (적용 시)
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Defense", ReplicatedUsing = OnRep_DamageReduction)
	FGameplayAttributeData DamageReduction;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, DamageReduction)


	// 약점 공격 여부 (Meta)
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Meta")
	FGameplayAttributeData IsCriticalHit;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, IsCriticalHit)

	// 최종 피해 계산
	float CalculateFinalDamage(bool bIsCritical, float TargetDamageReduction) const;

protected:
	UFUNCTION()
	virtual void OnRep_WeaponBaseDamage(const FGameplayAttributeData& OldWeaponBaseDamage);

	UFUNCTION()
	virtual void OnRep_WeaponDamageIncrease(const FGameplayAttributeData& OldWeaponDamageIncrease);

	UFUNCTION()
	virtual void OnRep_WeaponDamageMultiplier(const FGameplayAttributeData& OldWeaponDamageMultiplier);

	UFUNCTION()
	virtual void OnRep_BaseCriticalMultiplier(const FGameplayAttributeData& OldBaseCriticalMultiplier);

	UFUNCTION()
	virtual void OnRep_CriticalMultiplierAmplify(const FGameplayAttributeData& OldCriticalMultiplierAmplify);

	UFUNCTION()
	virtual void OnRep_CriticalMultiplierAdditional(const FGameplayAttributeData& OldCriticalMultiplierAdditional);

	UFUNCTION()
	virtual void OnRep_NormalMultiplier(const FGameplayAttributeData& OldNormalMultiplier);

	UFUNCTION()
	virtual void OnRep_FinalDamageIncrease(const FGameplayAttributeData& OldFinalDamageIncrease);

	UFUNCTION()
	virtual void OnRep_FinalDamageMultiplier(const FGameplayAttributeData& OldFinalDamageMultiplier);

	UFUNCTION()
	virtual void OnRep_FinalDamageAdditional(const FGameplayAttributeData& OldFinalDamageAdditional);

	UFUNCTION()
	virtual void OnRep_DamageReduction(const FGameplayAttributeData& OldDamageReduction);
};
