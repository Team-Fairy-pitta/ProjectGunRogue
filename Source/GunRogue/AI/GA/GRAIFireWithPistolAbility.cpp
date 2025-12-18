// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GA/GRAIFireWithPistolAbility.h"

UGRAIFireWithPistolAbility::UGRAIFireWithPistolAbility()
{
}

void UGRAIFireWithPistolAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ActorInfo->AvatarActor.Get()->HasAuthority())
	{
		PlayAttackMontageAndWaitTask();

		if (!PrepareFireContext())
		{
			EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, false);
		}
		
		SpawnBulletProjectile(GetAimDirection());
	}
}

