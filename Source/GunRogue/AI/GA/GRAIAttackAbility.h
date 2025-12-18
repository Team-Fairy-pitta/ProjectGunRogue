// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRGameplayAbility.h"
#include "GRAIAttackAbility.generated.h"

/**
 * 
 */

UCLASS()
class GUNROGUE_API UGRAIAttackAbility : public UGRGameplayAbility
{
	GENERATED_BODY()

public:
	UGRAIAttackAbility();
	
protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	void PlayAttackMontageAndWaitTask();
	void WaitAttackGameplayEventTask();
	void SpawnProjectile();
	void DetectByOverlapSphereMulti();
	void CauseDamage(AActor* Target);
	
	UFUNCTION()
	virtual void OnAttackTriggerNotify(FGameplayEventData Payload);

	UFUNCTION()
	void OnMontageEnded();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Animation")
	UAnimMontage* AttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Animation")
	FGameplayTag HitEventTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	FGameplayAbilitySpecHandle SavedSpecHandle;
	const FGameplayAbilityActorInfo* SavedActorInfo;
	FGameplayAbilityActivationInfo SavedActivationInfo;

	UPROPERTY(EditDefaultsOnly,Category = "Ability|Projectile",meta=(AllowPrivateAccess=true))
	TSubclassOf<AActor> ProjectileClass;
	
	UPROPERTY()
	TObjectPtr<AActor> Projectile;

	FName ProjectileSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Damage")
	float SphereRadius;
};
