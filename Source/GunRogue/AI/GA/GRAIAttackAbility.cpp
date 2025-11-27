// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GA/GRAIAttackAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UGRAIAttackAbility::UGRAIAttackAbility()
	:AttackMontage(nullptr)
	,SavedSpecHandle(FGameplayAbilitySpecHandle())
	,SavedActorInfo(nullptr)
	,SavedActivationInfo(FGameplayAbilityActivationInfo())
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGRAIAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	SavedSpecHandle = Handle;
	SavedActorInfo = ActorInfo;
	SavedActivationInfo = ActivationInfo;
	
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid() || !AttackMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
}

void UGRAIAttackAbility::PlayAttackMontageAndWaitTask()
{
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			AttackMontage,
			1.0f,      // 재생 속도
			NAME_None, // 시작 섹션
			true,      // StopWhenAbilityEnds
			0.0f       // 시작 위치
		);

	MontageTask->OnCompleted.AddDynamic(this, &UGRAIAttackAbility::OnMontageEnded);
	MontageTask->OnInterrupted.AddDynamic(this, &UGRAIAttackAbility::OnMontageEnded);
	MontageTask->OnCancelled.AddDynamic(this, &UGRAIAttackAbility::OnMontageEnded);
	MontageTask->OnBlendOut.AddDynamic(this, &UGRAIAttackAbility::OnMontageEnded);
	MontageTask->ReadyForActivation();
}

void UGRAIAttackAbility::WaitAttackGameplayEventTask()
{
	UAbilityTask_WaitGameplayEvent* WaitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		HitEventTag,
		SavedActorInfo->AvatarActor.Get(),  // ExternalTarget: 보통 자기 자신
		true,  // OnlyTriggerOnce
		false  // OnlyMatchExact
	);

	WaitEvent->EventReceived.AddDynamic(this, &UGRAIAttackAbility::OnHitNotify);
	WaitEvent->ReadyForActivation();
}

void UGRAIAttackAbility::OnHitNotify(FGameplayEventData Payload)
{
}

void UGRAIAttackAbility::OnMontageEnded()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, false);
}
