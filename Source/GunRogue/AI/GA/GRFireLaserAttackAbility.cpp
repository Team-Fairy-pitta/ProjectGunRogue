// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GA/GRFireLaserAttackAbility.h"
#include "AI/Character/GRAICharacter.h"
#include "AI/Controller/GRAIController.h"
#include "AI/Projectile/GRLaserProjectile.h"
#include "BehaviorTree/BlackboardComponent.h"

UGRFireLaserAttackAbility::UGRFireLaserAttackAbility()
{
	ProjectileSocketName = FName("SpawnLaserLocation");
	FireGameplayCueTagName = FName("GameplayCue.AI.Boss.FireLaser");
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
	
	LaunchProjectile();
}

void UGRFireLaserAttackAbility::LaunchProjectile()
{
	AGRLaserProjectile* AIProjectile=Cast<AGRLaserProjectile>(Projectile);
	if (!AIProjectile)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, false);
		return;
	}

	AGRAICharacter* AICharacter = Cast<AGRAICharacter>(SavedActorInfo->AvatarActor.Get());
	if (!AICharacter)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}
	
	AGRAIController* AICon=Cast<AGRAIController>(AICharacter->GetController());
	if (!AICon)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}
	
	UBlackboardComponent* BB=AICon->GetBlackboardComponent();
	if (!BB)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(AGRAIController::TargetPlayerKey));
	if (!TargetActor)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}

	if (!AICharacter->HasAuthority())
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}
	
	FVector StartLocation = AIProjectile->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();

	FVector LaunchDirection=(TargetLocation - StartLocation).GetSafeNormal();

	AIProjectile->Launch(LaunchDirection);
	
	Projectile=nullptr;

	ExecuteGameplayCueWhenStartFire();
}

