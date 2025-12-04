// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GA/GRBoulderTossAttackAbility.h"
#include "AI/Projectile/GRRockProjectile.h"
#include "AI/Character/GRLuwoAICharacter.h"
#include "AI/Controller/GRBossLuwoAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UGRBoulderTossAttackAbility::UGRBoulderTossAttackAbility()
{
	ProjectileSocketName = FName("SpawnRockLocation");
}

void UGRBoulderTossAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

void UGRBoulderTossAttackAbility::OnAttackTriggerNotify(FGameplayEventData Payload)
{
	Super::OnAttackTriggerNotify(Payload);

	SpawnProjectile();
	
	AGRRockProjectile* RockProjectile=Cast<AGRRockProjectile>(Projectile);
	if (!RockProjectile)
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
	
	FVector StartLocation = RockProjectile->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();

	FVector LaunchVelocity;
	const float ArcParam=0.5f;
	bool bHaveSolution = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		 GetWorld(),
		 LaunchVelocity,
		 StartLocation,
		 TargetLocation,
		 GetWorld()->GetGravityZ(),
		 ArcParam 
	);

	if (bHaveSolution)
	{
		RockProjectile->Throw(LaunchVelocity);
	}
	else
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}
	
	Projectile=nullptr;
	
}

