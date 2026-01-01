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
	GameplayCueTagName=FName("GameplayCue.AI.Boss.JumpAndAttack");
}

void UGRJumpAndAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ActorInfo->AvatarActor.Get()->HasAuthority())
	{
		JumpToTargetLocation();
	}
}

void UGRJumpAndAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	AGRLuwoAICharacter* Boss = Cast<AGRLuwoAICharacter>(SavedActorInfo->AvatarActor.Get());
	if (Boss)
	{
		if (Boss->OnLandedEvent.IsAlreadyBound(this, &UGRJumpAndAttackAbility::OnLanded))
		{
			Boss->OnLandedEvent.RemoveDynamic(this, &UGRJumpAndAttackAbility::OnLanded);
		}
	}
}

void UGRJumpAndAttackAbility::OnAttackTriggerNotify(FGameplayEventData Payload)
{
	Super::OnAttackTriggerNotify(Payload);

	DetectByOverlapSphereMulti();
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
	else
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}
	
	FVector LaunchVel;
	const float ArcParam=0.5f;
	bool bHaveSolution = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		Boss,
		LaunchVel,
		Start,
		TargetLocation,
		Boss -> GetWorld()->GetGravityZ(),
		ArcParam);
	
	if (!bHaveSolution)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}
	
	Boss->LaunchCharacter(LaunchVel, true, true);
	
	if (!Boss->OnLandedEvent.IsAlreadyBound(this, &UGRJumpAndAttackAbility::OnLanded))
	{
		Boss->OnLandedEvent.AddDynamic(this, &UGRJumpAndAttackAbility::OnLanded);
	}
}

void UGRJumpAndAttackAbility::OnLanded()
{
	PlayAttackMontageAndWaitTask();
	WaitAttackGameplayEventTask();
	ExecuteGameplayCueAtGround();
}
