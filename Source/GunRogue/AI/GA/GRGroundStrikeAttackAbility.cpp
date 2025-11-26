// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GA/GRGroundStrikeAttackAbility.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Character/GRCharacter.h"

UGRGroundStrikeAttackAbility::UGRGroundStrikeAttackAbility()
{
}

void UGRGroundStrikeAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PlayAttackMontageAndWaitTask();
	WaitAttackGameplayEventTask();
}

void UGRGroundStrikeAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UGRGroundStrikeAttackAbility::OnHitNotify(FGameplayEventData Payload)
{
	Super::OnHitNotify(Payload);
	
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
		FLinearColor::Yellow,                 // 히트 위치 색
		FLinearColor::Red,               // Trace 라인 색
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





