// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Robot/GA/GRRobot_SubAbility.h"
#include "AbilitySystemComponent.h"
#include "AI/Robot/Drone/GRDroneManagerComponent.h"

UGRRobot_SubAbility::UGRRobot_SubAbility()
{
}

void UGRRobot_SubAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UGRDroneManagerComponent* DroneMgr = Avatar->FindComponentByClass<UGRDroneManagerComponent>();
	if (!DroneMgr)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (!HasAuthority(&ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	DroneMgr->SpawnDrone();

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

