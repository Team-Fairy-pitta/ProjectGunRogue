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

public:
	// 로봇 피해량
	UPROPERTY(BlueprintReadOnly, Category = "Robot Attributes")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_Robot, Damage)

	// 로봇 지속시간
	UPROPERTY(BlueprintReadOnly, Category = "Robot Attributes")
	FGameplayAttributeData Duration;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_Robot, Duration)

	// 로봇 공격속도
	UPROPERTY(BlueprintReadOnly, Category = "Robot Attributes")
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_Robot, AttackSpeed)

	// 자폭 피해량
	UPROPERTY(BlueprintReadOnly, Category = "Robot Attributes")
	FGameplayAttributeData SelfDestructDamage;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_Robot, SelfDestructDamage)
	
	// 자폭 반경
	UPROPERTY(BlueprintReadOnly, Category = "Robot Attributes")
	FGameplayAttributeData SelfDestructRadius;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_Robot, SelfDestructRadius)

	// 기본 쿨타임
	UPROPERTY(BlueprintReadOnly, Category = "Robot Attributes")
	FGameplayAttributeData Cooldown;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_Robot, Cooldown)
};
