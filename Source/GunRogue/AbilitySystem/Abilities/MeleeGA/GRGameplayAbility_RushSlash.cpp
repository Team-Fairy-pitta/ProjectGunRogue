#include "AbilitySystem/Abilities/MeleeGA/GRGameplayAbility_RushSlash.h"

#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "AbilitySystem/Attributes/GRSkillAttributeSet_MeleeSkill.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Character/GRCharacter.h"

UGRGameplayAbility_RushSlash::UGRGameplayAbility_RushSlash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

const UGRSkillAttributeSet_MeleeSkill* UGRGameplayAbility_RushSlash::GetSkillSet() const
{
	const UGRAbilitySystemComponent* GRASC = Cast<UGRAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());

	return GRASC ? GRASC->GetSet<UGRSkillAttributeSet_MeleeSkill>() : nullptr;
}

bool UGRGameplayAbility_RushSlash::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	AGRCharacter* GRChar = Cast<AGRCharacter>(ActorInfo->AvatarActor.Get());
	if (!GRChar)
	{
		return false;
	}

	return true;
}

void UGRGameplayAbility_RushSlash::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const UGRSkillAttributeSet_MeleeSkill* SkillSet = GetSkillSet();
	if (!SkillSet)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AlreadyHitActors.Reset();
	PreviousActorLocation = ActorInfo->AvatarActor->GetActorLocation();

	StartDash();

	if (ActorInfo->IsNetAuthority())
	{
		StartHitCheck();
	}
}

void UGRGameplayAbility_RushSlash::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	StopHitCheck();

	if (DashTask)
	{
		DashTask->EndTask();
		DashTask = nullptr;
	}

	AlreadyHitActors.Reset();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGRGameplayAbility_RushSlash::StartDash()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		OnDashFinished();
		return;
	}

	const UGRSkillAttributeSet_MeleeSkill* SkillSet = GetSkillSet();
	if (!SkillSet)
	{
		OnDashFinished();
		return;
	}

	const float DashDuration = SkillSet->GetRushSlash_DashDuration();
	const float DashForce = SkillSet->GetRushSlash_DashForce();

	DashTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
		this,
		NAME_None,
		Avatar->GetActorForwardVector(),
		DashForce,
		DashDuration,
		false,
		nullptr,
		ERootMotionFinishVelocityMode::SetVelocity,
		FVector::ZeroVector,
		0.0f,
		false
	);

	DashTask->OnFinish.AddDynamic(this, &UGRGameplayAbility_RushSlash::OnDashFinished);
	DashTask->ReadyForActivation();
}

void UGRGameplayAbility_RushSlash::StopDash()
{
	if (DashTask)
	{
		DashTask->EndTask();
		DashTask = nullptr;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (OwnerCharacter)
	{
		if (UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement())
		{
			MoveComp->StopMovementImmediately();
		}
	}
}

void UGRGameplayAbility_RushSlash::StartHitCheck()
{
	const UGRSkillAttributeSet_MeleeSkill* SkillSet = GetSkillSet();
	if (!SkillSet)
	{
		return;
	}

	const float HitCheckInterval = SkillSet->GetRushSlash_HitCheckInterval();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			HitCheckTimerHandle,
			FTimerDelegate::CreateUObject(this, &UGRGameplayAbility_RushSlash::PerformHitCheck, CurrentActorInfo),
			HitCheckInterval,
			true
		);
	}
}

void UGRGameplayAbility_RushSlash::StopHitCheck()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HitCheckTimerHandle);
	}
}

void UGRGameplayAbility_RushSlash::PerformHitCheck(const FGameplayAbilityActorInfo* ActorInfo)
{
	AActor* Avatar = ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr;
	if (!Avatar)
	{
		return;
	}

	UGRAbilitySystemComponent* GRASC = Cast<UGRAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (!GRASC)
	{
		return;
	}

	const UGRSkillAttributeSet_MeleeSkill* SkillSet = GetSkillSet();
	if (!SkillSet) 
	{
		return;
	}

	const float HitRadius = SkillSet->GetRushSlash_HitRadius();

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
		QueryParams
	);

	bool bShouldStopDash = false;

	if (bAnyHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || AlreadyHitActors.Contains(HitActor))
			{
				continue;
			}

			if (HitActor->IsA(AGRCharacter::StaticClass()))
			{
				continue;
			}

			AlreadyHitActors.Add(HitActor);

			ApplyKnockbackToTarget(HitActor, Avatar->GetActorLocation());

			const float FinalDamage = SkillSet->GetRushSlash_BaseDamage() * SkillSet->GetRushSlash_DamageMultiplier();

			ApplyDamageToTarget(HitActor, Hit, FinalDamage);

			bShouldStopDash = true;
			break;
		}
	}

	PreviousActorLocation = CurrentLocation;

	if (bShouldStopDash)
	{
		StopHitCheck();
		StopDash();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
}

void UGRGameplayAbility_RushSlash::ApplyKnockbackToTarget(
	AActor* TargetActor,
	const FVector& DashStartLocation) const
{
	ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
	if (!TargetCharacter)
	{
		return;
	}

	const UGRSkillAttributeSet_MeleeSkill* SkillSet = GetSkillSet();
	if (!SkillSet)
	{
		return;
	}

	const float Mult = SkillSet->GetRushSlash_KnockbackUpgradeMultiplier();
	const float KnockbackStrength = SkillSet->GetRushSlash_KnockbackStrength() * Mult;
	const float KnockbackUpward = SkillSet->GetRushSlash_KnockbackUpward() * Mult;

	const FVector KnockbackDirection = (TargetCharacter->GetActorLocation() - DashStartLocation).GetSafeNormal2D();

	TargetCharacter->LaunchCharacter(
		KnockbackDirection * KnockbackStrength + FVector::UpVector * KnockbackUpward,
		true,
		true
	);
}

void UGRGameplayAbility_RushSlash::OnDashFinished()
{
	if (CurrentActorInfo && CurrentActorInfo->IsNetAuthority())
	{
		StopHitCheck();
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGRGameplayAbility_RushSlash::ApplyDamageToTarget(AActor* TargetActor, const FHitResult& Hit, float Damage) const
{
	if (!TargetActor || !DamageEffect)
	{
		return;
	}

	UGRAbilitySystemComponent* SourceASC = Cast<UGRAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (!SourceASC)
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	if (!TargetASC)
	{
		return;
	}

	if (TargetActor->IsA(AGRCharacter::StaticClass()))
	{
		return;
	}
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar || !Avatar->HasAuthority())
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(Avatar);
	EffectContext.AddHitResult(Hit);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffect, 1.0f, EffectContext);
	if (!SpecHandle.IsValid())
	{
		return;
	}

	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Attribute.Data.Damage")), Damage);

	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}
