// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/GA/GRAIAttackAbility.h"
#include "GRFireLaserAttackAbility.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRFireLaserAttackAbility : public UGRAIAttackAbility
{
	GENERATED_BODY()
	
public:
	UGRFireLaserAttackAbility();
	
protected:
   	virtual void ActivateAbility(
   		const FGameplayAbilitySpecHandle Handle,
   		const FGameplayAbilityActorInfo* ActorInfo,
   		const FGameplayAbilityActivationInfo ActivationInfo,
   		const FGameplayEventData* TriggerEventData) override;
   
   	virtual void OnAttackTriggerNotify(FGameplayEventData Payload) override;

	void LaunchProjectile();
};
