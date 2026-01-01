// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRGameplayAbility.h"
#include "GRRobot_SubAbility.generated.h"

/**
 * 
 */

class AGRDrone;

class UGRSkillAttributeSet_Robot;

UCLASS()
class GUNROGUE_API UGRRobot_SubAbility : public UGRGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGRRobot_SubAbility();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	
	virtual void ApplyCooldown(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

private:
	float GetCooldown(const FGameplayAbilityActorInfo* ActorInfo) const;
};
