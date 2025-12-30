// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Robot/GA/GRRobot_MainAbility.h"
#include "AI/Robot/Drone/GRDroneManagerComponent.h"

UGRRobot_MainAbility::UGRRobot_MainAbility()
{
}

void UGRRobot_MainAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
	
	if (!HasAuthority(&ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
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
	
	if (DroneMgr->GetDrones().Num() == 0)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	DroneMgr->CommandAllDronesAttack(GetAimDirection());

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

FVector UGRRobot_MainAbility::GetAimDirection()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		return FVector::ZeroVector;
	}

	APlayerController* PC = Cast<APlayerController>(Cast<APawn>(Avatar)->GetController());

	if (!PC)
	{
		return FVector::ZeroVector;
	}

	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	return CamRot.Vector();
}
