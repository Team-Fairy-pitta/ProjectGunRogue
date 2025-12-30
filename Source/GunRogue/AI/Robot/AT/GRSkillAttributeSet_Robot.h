// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "GRSkillAttributeSet_Robot.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRSkillAttributeSet_Robot : public UAttributeSet
{
	GENERATED_BODY()

public:
	UGRSkillAttributeSet_Robot();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// 로봇 피해량
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Damage, Category = "Robot Attributes")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_Robot, Damage)

	// 로봇 지속시간 (초)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Duration, Category = "Robot Attributes")
	FGameplayAttributeData Duration;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_Robot, Duration)

	// 로봇 공격속도. 1.0이 보통. 값이 커질수록 빨라짐. 2.0이 되면 공격속도 두배.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackSpeed, Category = "Robot Attributes")
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_Robot, AttackSpeed)

	// 자폭 피해량
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SelfDestructDamage, Category = "Robot Attributes")
	FGameplayAttributeData SelfDestructDamage;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_Robot, SelfDestructDamage)
	
	// 자폭 반경
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SelfDestructRadius, Category = "Robot Attributes")
	FGameplayAttributeData SelfDestructRadius;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_Robot, SelfDestructRadius)

	// 기본 쿨타임 (초)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MainCooldown, Category = "Robot Attributes")
	FGameplayAttributeData MainCooldown;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_Robot, MainCooldown)

	// 기본 쿨타임 (초)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SubCooldown, Category = "Robot Attributes")
	FGameplayAttributeData SubCooldown;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_Robot, SubCooldown)

	UFUNCTION()
	virtual void OnRep_Damage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Duration(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_AttackSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_SelfDestructDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_SelfDestructRadius(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MainCooldown(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_SubCooldown(const FGameplayAttributeData& OldValue);

};
