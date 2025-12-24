#include "TestMeleeCharacter/TestGA_RushSlash.h"

#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

UTestGA_RushSlash::UTestGA_RushSlash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	Tag_AmmoRestoreOnHit			= FGameplayTag::RequestGameplayTag(TEXT("Augment.RushSlash.AmmoRestore"));
	Tag_IncreaseKnockbackAndDamage	= FGameplayTag::RequestGameplayTag(TEXT("Augment.RushSlash.IncreaseKnockback"));
	Tag_PierceTargets				= FGameplayTag::RequestGameplayTag(TEXT("Augment.RushSlash.PierceTargets"));

	DataTag_AmmoRestore				= FGameplayTag::RequestGameplayTag(TEXT("Data.AmmoRestore"));
}

void UTestGA_RushSlash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!HasAuthority(&ActivationInfo) ||
		!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AlreadyHitActors.Reset();
	PreviousActorLocation = ActorInfo->AvatarActor->GetActorLocation();

	StartDash();
	StartHitCheck();
}

void UTestGA_RushSlash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{ 
	StopHitCheck();
	AlreadyHitActors.Reset();
	DashTask = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UTestGA_RushSlash::StartDash()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		OnDashFinished();
		return;
	}

	DashTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this,                          
		NAME_None,                          
		GetAvatarActorFromActorInfo()->GetActorForwardVector(),
		DashStrength,                      
		DashDuration,                       
		false,                              
		nullptr,                           
		ERootMotionFinishVelocityMode::SetVelocity, 
		FVector::ZeroVector,            
		0.0f,                              
		false                            
	);

	DashTask->OnFinish.AddDynamic(this, &UTestGA_RushSlash::OnDashFinished);
	DashTask->ReadyForActivation();
}

void UTestGA_RushSlash::StartHitCheck()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			HitCheckTimerHandle,
			FTimerDelegate::CreateUObject(this, &UTestGA_RushSlash::PerformHitCheck, CurrentActorInfo),
			HitCheckInterval,
			true);
	}
}

void UTestGA_RushSlash::StopHitCheck()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HitCheckTimerHandle);
	}
}

void UTestGA_RushSlash::PerformHitCheck(const FGameplayAbilityActorInfo* ActorInfo)
{
	AActor* Avatar = ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr;
	if (!Avatar)
	{
		return;
	}

	UAbilitySystemComponent* GRASC = Cast<UGRAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (!GRASC)
	{
		return;
	}

	const bool bHasAmmoRestoreAugment = GRASC->HasMatchingGameplayTag(Tag_AmmoRestoreOnHit);

	const bool bHasKnockbackUpgrade = GRASC->HasMatchingGameplayTag(Tag_IncreaseKnockbackAndDamage);

	const bool bHasPierceAugment = GRASC->HasMatchingGameplayTag(Tag_PierceTargets);

	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Avatar);

	const FVector CurrentLocation = Avatar->GetActorLocation();

	const bool bAnyHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		PreviousActorLocation,
		CurrentLocation,
		FQuat::Identity,
		HitChannel,
		FCollisionShape::MakeSphere(HitRadius),
		QueryParams);

	bool bHitAtLeastOneNewTarget = false;

	if (bAnyHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || AlreadyHitActors.Contains(HitActor))
			{
				continue;
			}

			AlreadyHitActors.Add(HitActor);
			bHitAtLeastOneNewTarget = true;

			ApplyKnockbackToTarget(
				HitActor,
				Avatar->GetActorLocation(),
				bHasPierceAugment,
				bHasKnockbackUpgrade);
		}
	}

	if (bHasAmmoRestoreAugment)
	{
		ApplyAmmoRestoreIfNeeded(bHitAtLeastOneNewTarget);
	}

	PreviousActorLocation = CurrentLocation;
}

void UTestGA_RushSlash::ApplyKnockbackToTarget(AActor* TargetActor, const FVector& DashStartLocation, bool bIsPiercing, bool bIsKnockbackUpgraded) const
{
	if (bIsPiercing)
	{
		return;
	}

	ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
	if (!TargetCharacter) 
	{ 
		return; 
	}

	FVector KnockbackDirection = (TargetCharacter->GetActorLocation() - DashStartLocation).GetSafeNormal2D();

	float FinalKnockbackStrength = BaseKnockbackStrength;
	float FinalUpwardForce = BaseKnockbackUpward;

	if (bIsKnockbackUpgraded)
	{
		FinalKnockbackStrength *= KnockbackMultiplierWhenUpgraded;
		FinalUpwardForce *= KnockbackMultiplierWhenUpgraded;
	}

	TargetCharacter->LaunchCharacter(
		KnockbackDirection * FinalKnockbackStrength +
		FVector::UpVector * FinalUpwardForce,
		true,
		true);
}

void UTestGA_RushSlash::ApplyAmmoRestoreIfNeeded(bool bAnyTargetHit)
{
	if (!bAnyTargetHit || !AmmoRestoreEffect) 
	{ 
		return; 
	}

	FGameplayEffectSpecHandle SpecHandle =
		MakeOutgoingGameplayEffectSpec(AmmoRestoreEffect, GetAbilityLevel());

	if (!SpecHandle.IsValid())
	{
		return;
	}

	SpecHandle.Data->SetSetByCallerMagnitude(DataTag_AmmoRestore,AmmoRestoreAmount);

	ApplyGameplayEffectSpecToOwner(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		SpecHandle);
}

void UTestGA_RushSlash::OnDashFinished()
{
	StopHitCheck();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
