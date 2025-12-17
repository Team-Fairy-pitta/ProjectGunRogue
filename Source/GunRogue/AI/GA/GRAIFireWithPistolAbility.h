// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/GA/GRAIWeaponFireAbility.h"
#include "GRAIFireWithPistolAbility.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRAIFireWithPistolAbility : public UGRAIWeaponFireAbility
{
	GENERATED_BODY()
	
public:
	UGRAIFireWithPistolAbility();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
};
