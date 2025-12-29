// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "GRSkillAttributeSet_GiantBomb.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * GiantBomb 스킬 전용 AttributeSet
 * 캐릭터 앞에 거대한 폭탄을 소환 후 일정 시간 뒤 폭발. (적의 투사체, 히트스캔 막기 가능)
 */

UCLASS()
class GUNROGUE_API UGRSkillAttributeSet_GiantBomb : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UGRSkillAttributeSet_GiantBomb();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ======== 폭탄 능력치 ========

	UPROPERTY(BlueprintReadOnly, Category = "Skill|GiantBomb", ReplicatedUsing = OnRep_BaseDamage)
	FGameplayAttributeData BaseDamage;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_GiantBomb, BaseDamage)

	UPROPERTY(BlueprintReadOnly, Category = "Skill|GiantBomb", ReplicatedUsing = OnRep_ExplosionRadius)
	FGameplayAttributeData ExplosionRadius;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_GiantBomb, ExplosionRadius)

	UPROPERTY(BlueprintReadOnly, Category = "Skill|GiantBomb", ReplicatedUsing = OnRep_ExplosionFalloff)
	FGameplayAttributeData ExplosionFalloff;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_GiantBomb, ExplosionFalloff)

	// ======== 폭탄 설정 ========

	UPROPERTY(BlueprintReadOnly, Category = "Skill|GiantBomb", ReplicatedUsing = OnRep_FuseTime)
	FGameplayAttributeData FuseTime;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_GiantBomb, FuseTime)

	UPROPERTY(BlueprintReadOnly, Category = "Skill|GiantBomb", ReplicatedUsing = OnRep_KnockbackForce)
	FGameplayAttributeData KnockbackForce;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_GiantBomb, KnockbackForce)

	// ======== 쿨타임 ========
	
	UPROPERTY(BlueprintReadOnly, Category = "Skill|GiantBomb", ReplicatedUsing = OnRep_BaseCooldown)
	FGameplayAttributeData BaseCooldown;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_GiantBomb, BaseCooldown)

	UPROPERTY(BlueprintReadOnly, Category = "Skill|GiantBomb", ReplicatedUsing = OnRep_CooldownReduction)
	FGameplayAttributeData CooldownReduction;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_GiantBomb, CooldownReduction)

protected:
	UFUNCTION()
	virtual void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_ExplosionRadius(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_ExplosionFalloff(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_FuseTime(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_KnockbackForce(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_BaseCooldown(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_CooldownReduction(const FGameplayAttributeData& OldValue);

};
