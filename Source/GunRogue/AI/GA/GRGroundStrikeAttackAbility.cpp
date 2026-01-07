// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GA/GRGroundStrikeAttackAbility.h"
#include "AbilitySystemComponent.h"
#include "AI/Character/GRLuwoAICharacter.h"

UGRGroundStrikeAttackAbility::UGRGroundStrikeAttackAbility()
{
	GameplayCueTagName=FName("GameplayCue.AI.Boss.GroundStrike");
}

void UGRGroundStrikeAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ActorInfo->AvatarActor.Get()->HasAuthority())
	{
		PlayAttackMontageAndWaitTask();
		WaitAttackGameplayEventTask();

		AGRLuwoAICharacter* BossLuwo = Cast<AGRLuwoAICharacter>(GetAvatarActorFromActorInfo());
		if (BossLuwo)
		{
			BossLuwo->StartFlyingGC();
		}
	}
}

void UGRGroundStrikeAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	AGRLuwoAICharacter* BossLuwo = Cast<AGRLuwoAICharacter>(GetAvatarActorFromActorInfo());
	if (BossLuwo)
	{
		BossLuwo->StopFlyingGC();
	}
}

void UGRGroundStrikeAttackAbility::OnAttackTriggerNotify(FGameplayEventData Payload)
{
	Super::OnAttackTriggerNotify(Payload);
	
	DetectByOverlapSphereMulti();
	ExecuteGameplayCueAtGround();
}





