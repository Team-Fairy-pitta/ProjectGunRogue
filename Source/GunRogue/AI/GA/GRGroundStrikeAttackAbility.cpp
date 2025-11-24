// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GA/GRGroundStrikeAttackAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

UGRGroundStrikeAttackAbility::UGRGroundStrikeAttackAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGRGroundStrikeAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid() || !AttackMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 몽타주 재생 + 이벤트 기다리기
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

	MontageTask->OnCompleted.AddDynamic(this, &UGRGroundStrikeAttackAbility::OnMontageEnded);
	MontageTask->OnInterrupted.AddDynamic(this, &UGRGroundStrikeAttackAbility::OnMontageEnded);
	MontageTask->OnCancelled.AddDynamic(this, &UGRGroundStrikeAttackAbility::OnMontageEnded);
	MontageTask->OnBlendOut.AddDynamic(this, &UGRGroundStrikeAttackAbility::OnMontageEnded);
	MontageTask->ReadyForActivation();

	// 이벤트 대기
	UAbilityTask_WaitGameplayEvent* WaitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		HitEventTag,
		ActorInfo->AvatarActor.Get(),  // ExternalTarget: 보통 자기 자신
		true,  // OnlyTriggerOnce
		false  // OnlyMatchExact
	);

	WaitEvent->EventReceived.AddDynamic(this, &UGRGroundStrikeAttackAbility::OnHitNotify);
	WaitEvent->ReadyForActivation();
}

void UGRGroundStrikeAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGRGroundStrikeAttackAbility::OnHitNotify(FGameplayEventData Payload)
{
	// 데미지 로직
	AActor* Instigator = GetAvatarActorFromActorInfo();
	if (!Instigator) return;

	// 예: Sphere Trace로 범위 데미지 판정
	FVector Origin = Instigator->GetActorLocation();
	float Radius = 300.f;
	TArray<FHitResult> Hits;

	UKismetSystemLibrary::SphereTraceMulti(
		Instigator->GetWorld(),
		Origin,
		Origin,
		Radius,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false,
		TArray<AActor*>({Instigator}),
		EDrawDebugTrace::ForDuration,      // 디버그 그리기 타입
		Hits,
		true,
		FLinearColor::Yellow,                 // Trace 라인 색
		FLinearColor::Red,               // 히트 위치 색
		1.0f                                // 화면에 표시되는 시간 (초)
	);

	//GRCharacter에게만 데미지 주기.
	for (auto& Hit : Hits)
	{
		AActor* Other = Hit.GetActor();
		if (!Other) continue;

		IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Other);
		if (!ASI) continue;

		UAbilitySystemComponent* TargetASC = ASI->GetAbilitySystemComponent();
		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
		if (!TargetASC || !SourceASC) continue;

		FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
		Context.AddSourceObject(Instigator);

		FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), Context);
		if (Spec.IsValid())
		{
			SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
		}
	}
}


void UGRGroundStrikeAttackAbility::OnMontageEnded()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, false);
}




