// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/GA/GRAIAttackAbility.h"
#include "GRBoulderTossAttackAbility.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRBoulderTossAttackAbility : public UGRAIAttackAbility
{
	GENERATED_BODY()
	
public:
	UGRBoulderTossAttackAbility();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	virtual void OnAttackTriggerNotify(FGameplayEventData Payload) override;
	
	void LaunchProjectile();
};
