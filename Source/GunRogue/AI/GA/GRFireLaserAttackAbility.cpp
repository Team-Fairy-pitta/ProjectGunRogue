// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GA/GRFireLaserAttackAbility.h"
#include "AI/Character/GRLuwoAICharacter.h"
#include "AI/Controller/GRBossLuwoAIController.h"
#include "AI/Projectile/GRLaserProjectile.h"
#include "BehaviorTree/BlackboardComponent.h"

UGRFireLaserAttackAbility::UGRFireLaserAttackAbility()
{
	ProjectileSocketName = FName("SpawnLaserLocation");
}

void UGRFireLaserAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ActorInfo->AvatarActor.Get()->HasAuthority())
	{
		PlayAttackMontageAndWaitTask();
		WaitAttackGameplayEventTask();
	}
}

void UGRFireLaserAttackAbility::OnAttackTriggerNotify(FGameplayEventData Payload)
{
	Super::OnAttackTriggerNotify(Payload);

	SpawnProjectile();
	
	AGRLaserProjectile* LaserProjectile=Cast<AGRLaserProjectile>(Projectile);
	if (!LaserProjectile)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, false);
		return;
	}

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

	if (!Boss->HasAuthority())
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}
	
	FVector StartLocation = LaserProjectile->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();

	FVector LaunchDirection=(TargetLocation - StartLocation).GetSafeNormal();

	LaserProjectile->Launch(LaunchDirection);
	
	Projectile=nullptr;
	
}
