// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GA/GRAIFireWithRifleAbility.h"

UGRAIFireWithRifleAbility::UGRAIFireWithRifleAbility()
{
}

void UGRAIFireWithRifleAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
		
		StartBurstFire();
	}
}

void UGRAIFireWithRifleAbility::StartBurstFire()
{
	BurstShotIndex = 0;
	
	GetWorld()->GetTimerManager().SetTimer(
		BurstTimerHandle,
		this,
		&UGRAIFireWithRifleAbility::FireBurstShot,
		BurstInterval,
		true
	);
}

void UGRAIFireWithRifleAbility::FireBurstShot()
{
	if (BurstShotIndex >= BurstCount)
	{
		GetWorld()->GetTimerManager().ClearTimer(BurstTimerHandle);
		return;
	}

	SpawnBulletProjectile(GetAimDirection());
	BurstShotIndex++;
}
