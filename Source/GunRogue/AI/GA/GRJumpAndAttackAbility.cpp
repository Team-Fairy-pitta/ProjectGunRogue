// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GA/GRJumpAndAttackAbility.h"
#include "AI/Character/GRLuwoAICharacter.h"
#include "AI/Controller/GRBossLuwoAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "AbilitySystemComponent.h"
#include "Character/GRCharacter.h"

UGRJumpAndAttackAbility::UGRJumpAndAttackAbility()
{
}

void UGRJumpAndAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	JumpToTargetLocation();
}

void UGRJumpAndAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	AGRLuwoAICharacter* Boss = Cast<AGRLuwoAICharacter>(SavedActorInfo->AvatarActor.Get());
	if (Boss)
	{
		Boss->OnLandedEvent.RemoveDynamic(this, &UGRJumpAndAttackAbility::OnLanded);
	}
}

void UGRJumpAndAttackAbility::OnHitNotify(FGameplayEventData Payload)
{
	Super::OnHitNotify(Payload);
	
	AActor* Instigator = GetAvatarActorFromActorInfo();
	if (!Instigator) return;

	// 예: 캡슐모양의 원기둥 비슷 모양 -막상 보니 거의 구의 형태와 비슷함
	float Height=100.0f;
	FVector Start = Instigator->GetActorLocation() - FVector(0,0, Height);
	FVector End = Instigator->GetActorLocation() + FVector(0,0, Height);     
	float Radius = 300.f;
	float HalfHeight = Height * 0.5f;

	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(Radius, HalfHeight);

	TArray<FHitResult> Hits;
	bool bHit = GetWorld()->SweepMultiByChannel(
		Hits,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,  
		CapsuleShape,
		FCollisionQueryParams(TEXT("MySweepTrace"), /*bTraceComplex=*/false, Instigator)
	);

	//NOTE : 디버깅 드로우
#if WITH_EDITOR
	DrawDebugCapsule(
		GetWorld(),
		(Start + End) * 0.5f,         // 캡슐의 센터 위치
		HalfHeight,
		Radius,
		FQuat::Identity,
		FColor::Red,
		/*bPersistentLines=*/false,
		/*LifeTime=*/1.0f,
		/*DepthPriority=*/0,
		/*Thickness=*/2.0f
	);
#endif
	
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

void UGRJumpAndAttackAbility::JumpToTargetLocation()
{
	AGRLuwoAICharacter* Boss = Cast<AGRLuwoAICharacter>(SavedActorInfo->AvatarActor.Get());
	if (!Boss)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}
	
	AGRBossLuwoAIController* BossCon=Cast<AGRBossLuwoAIController>(Boss->GetController());
	if (!BossCon)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}
	
	UBlackboardComponent* BB=BossCon->GetBlackboardComponent();
	if (!BB)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(AGRBossLuwoAIController::TargetPlayerKey));
	if (!TargetActor)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}

	FVector Start = Boss->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();
	
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	FNavLocation NavLoc;
	if (NavSys && NavSys->ProjectPointToNavigation(TargetLocation, NavLoc))
	{
		TargetLocation = NavLoc.Location;
	}
	
	FVector LaunchVel;
	bool bHaveSolution = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		Boss,
		LaunchVel,
		Start,
		TargetLocation,
		/*OverrideGravityZ*/ Boss -> GetWorld()->GetGravityZ(),
		/*곡선의 높이 조절 -낮으면 더 위로, 높으면 더 낮게*/ 0.5f);
	
	if (!bHaveSolution)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}
	
	Boss->LaunchCharacter(LaunchVel, true, true);
	
	Boss->OnLandedEvent.AddDynamic(this, &UGRJumpAndAttackAbility::OnLanded);
}

void UGRJumpAndAttackAbility::OnLanded()
{
	PlayAttackMontageAndWaitTask();
	WaitAttackGameplayEventTask();
}
