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
	// 공격 애니메이션 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Animation")
	UAnimMontage* AttackMontage;

	// 노티파이 태그: 애니메이션 중에 발생하는 이벤트 (예: 땅을 강타할 때)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Animation")
	FGameplayTag HitEventTag;

	// 데미지용 GameplayEffect 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Effects")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
