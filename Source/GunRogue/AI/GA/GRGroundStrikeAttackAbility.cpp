// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GA/GRGroundStrikeAttackAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Character/GRCharacter.h"

UGRGroundStrikeAttackAbility::UGRGroundStrikeAttackAbility()
	:AttackMontage(nullptr)
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
	AActor* Instigator = GetAvatarActorFromActorInfo();
	if (!Instigator) return;

	// 예: Sphere Trace로 범위 데미지 판정
	//TODO : 원기둥 모양으로, 범위 거리 추후 변경
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
		FLinearColor::Red,                 // 히트 위치 색
		FLinearColor::Yellow,               // Trace 라인 색
		1.0f                                // 화면에 표시되는 시간 (초)
	);
	
	for (auto& Hit : Hits)
	{
		AActor* Other = Hit.GetActor();
		if (!Other)
		{
			continue;
		}
		
		AGRCharacter* PlayerChar=Cast<AGRCharacter>(Other);
		if (!PlayerChar)
		{
			continue;
		}

		IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PlayerChar);
		if (!ASI)
		{
			continue;
		}
		
		UAbilitySystemComponent* PlayerASC = ASI->GetAbilitySystemComponent();
		if (!PlayerASC)
		{
			continue;
		}
		
		UAbilitySystemComponent* AIASC = GetAbilitySystemComponentFromActorInfo();
		if (!AIASC)
		{
			continue;
		}
		
		FGameplayEffectContextHandle Context = AIASC->MakeEffectContext();
		Context.AddSourceObject(Instigator);

		FGameplayEffectSpecHandle Spec = AIASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), Context);
		if (Spec.IsValid())
		{
			AIASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), PlayerASC);
		}
	}
}


void UGRGroundStrikeAttackAbility::OnMontageEnded()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, false);
}




