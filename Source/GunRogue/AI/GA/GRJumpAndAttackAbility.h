// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/GA/GRAIAttackAbility.h"
#include "GRJumpAndAttackAbility.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRJumpAndAttackAbility : public UGRAIAttackAbility
{
	GENERATED_BODY()

public:
	UGRJumpAndAttackAbility();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void OnHitNotify(FGameplayEventData Payload) override;

private:
	void JumpToTargetLocation();

	UFUNCTION()
	void OnLanded();

private:
	FDelegateHandle LandedDelegateHandle;
};
