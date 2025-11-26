// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRGameplayAbility.h"
#include "GRGroundStrikeAttackAbility.generated.h"

/**
 * 
 */

UCLASS()
class GUNROGUE_API UGRGroundStrikeAttackAbility : public UGRGameplayAbility
{
	GENERATED_BODY()

public:
	UGRGroundStrikeAttackAbility();

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
	
	UFUNCTION()
	void OnHitNotify(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageEnded();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Animation")
	UAnimMontage* AttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Animation")
	FGameplayTag HitEventTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Effects")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
