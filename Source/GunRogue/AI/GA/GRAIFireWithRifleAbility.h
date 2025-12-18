// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/GA/GRAIWeaponFireAbility.h"
#include "GRAIFireWithRifleAbility.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRAIFireWithRifleAbility : public UGRAIWeaponFireAbility
{
	GENERATED_BODY()

public:
	UGRAIFireWithRifleAbility();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

private:
	void StartBurstFire();
	void FireBurstShot();

private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	int32 BurstCount = 3;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	float BurstInterval = 0.08f; 

	FTimerHandle BurstTimerHandle;
	int32 BurstShotIndex = 0;

	
};
