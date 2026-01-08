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
#include "AI/Character/GRAICharacter.h"

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

	const FVector CurrentLocation = Avatar->GetActorLocation();
	const FVector TraceStart = PreviousActorLocation;
	const FVector TraceEnd = CurrentLocation;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(RushSlashHit), false, Avatar);
	QueryParams.bReturnPhysicalMaterial = false;

	TArray<FHitResult> HitResults;
	const bool bAnyHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		TraceStart,
		TraceEnd,
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
			AGRAICharacter* AITarget = Cast<AGRAICharacter>(HitActor);
			if (!AITarget)
			{
				continue;
			}

			AlreadyHitActors.Add(HitActor);

			ApplyKnockbackToTarget(AITarget, Avatar->GetActorLocation());

			const float FinalDamage = SkillSet->GetRushSlash_BaseDamage() * SkillSet->GetRushSlash_DamageMultiplier();
			ApplyDamageToTarget(AITarget, Hit, FinalDamage);

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
	const FVector& DashStartLocation)
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

	UCharacterMovementComponent* MoveComp = TargetCharacter->GetCharacterMovement();
	if (!MoveComp)
	{
		return;
	}

	const float Mult = SkillSet->GetRushSlash_KnockbackUpgradeMultiplier();
	const float KnockbackStrength = SkillSet->GetRushSlash_KnockbackStrength() * Mult;
	const float KnockbackUpward = SkillSet->GetRushSlash_KnockbackUpward() * Mult;

	const FVector KnockbackDirection = (TargetCharacter->GetActorLocation() - DashStartLocation).GetSafeNormal2D();
	const FVector KnockbackVelocity = KnockbackDirection * KnockbackStrength + FVector::UpVector * KnockbackUpward;

	const EMovementMode PrevMode = MoveComp->MovementMode;
	const bool bWasMovementNone = (PrevMode == MOVE_None);

	if (bWasMovementNone)
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}

	MoveComp->StopMovementImmediately();
	MoveComp->AddImpulse(KnockbackVelocity, true);

	if (!bWasMovementNone)
	{
		return;
	}

	UWorld* World = TargetCharacter->GetWorld();
	if (!World)
	{
		return;
	}

	TWeakObjectPtr<ACharacter> WeakTarget = TargetCharacter;

	struct FRestoreState
	{
		bool bEnteredFalling = false;
		float Elapsed = 0.f;
		FTimerHandle Handle;
	};

	const TSharedPtr<FRestoreState> State = MakeShared<FRestoreState>();
	const float PollInterval = 0.02f;
	const float MaxHoldSeconds = 0.5f;

	World->GetTimerManager().SetTimer(
		State->Handle,
		FTimerDelegate::CreateLambda([WeakTarget, PrevMode, World, State, PollInterval, MaxHoldSeconds]()
			{
				// 타이머 종료 + 이동모드 복구를 한 군데로 모음
				auto RestoreAndStop = [World, State, PrevMode](UCharacterMovementComponent* CMC)
					{
						if (CMC)
						{
							CMC->SetMovementMode(PrevMode);
						}
						if (World)
						{
							World->GetTimerManager().ClearTimer(State->Handle);
						}
					};

				if (!World)
				{
					return;
				}

				if (!WeakTarget.IsValid())
				{
					World->GetTimerManager().ClearTimer(State->Handle);
					return;
				}

				UCharacterMovementComponent* CMC = WeakTarget->GetCharacterMovement();
				if (!CMC)
				{
					World->GetTimerManager().ClearTimer(State->Handle);
					return;
				}

				State->Elapsed += PollInterval;

				// 1) 아직 Falling에 들어간 적이 없으면: Falling 진입 기다림
				if (!State->bEnteredFalling)
				{
					if (CMC->IsFalling())
					{
						State->bEnteredFalling = true;
						return;
					}

					// Falling이 안 생기는 케이스(수평 넉백 등) 대비: 상한으로 복구
					if (State->Elapsed >= MaxHoldSeconds)
					{
						RestoreAndStop(CMC);
					}
					return;
				}

				// 2) Falling에 들어갔으면: 착지(= Falling 종료)까지 기다렸다가 복구
				if (!CMC->IsFalling())
				{
					RestoreAndStop(CMC);
					return;
				}

				// 3) 공중에서 너무 오래 머무르는 경우도 상한으로 복구
				if (State->Elapsed >= MaxHoldSeconds)
				{
					RestoreAndStop(CMC);
					return;
				}
			}),
		PollInterval,
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

	if (!TargetActor->IsA(AGRAICharacter::StaticClass()))
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
