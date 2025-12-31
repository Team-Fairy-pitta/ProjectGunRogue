#include "AbilitySystem/Abilities/MeleeGA/GRGameplayAbility_BladeWaveFire.h"

#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRSkillAttributeSet_MeleeSkill.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

#include "Engine/World.h"
#include "GameFramework/Character.h"

UGRGameplayAbility_BladeWaveFire::UGRGameplayAbility_BladeWaveFire()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	Tag_BladeWaveMode = FGameplayTag::RequestGameplayTag(TEXT("State.BladeWaveMode"));

	ActivationRequiredTags.AddTag(Tag_BladeWaveMode);
}

UGRAbilitySystemComponent* UGRGameplayAbility_BladeWaveFire::GetGRASC() const
{
	return Cast<UGRAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
}

const UGRSkillAttributeSet_MeleeSkill* UGRGameplayAbility_BladeWaveFire::GetSkillSet() const
{
	const UGRAbilitySystemComponent* GRASC = GetGRASC();
	return GRASC ? GRASC->GetSet<UGRSkillAttributeSet_MeleeSkill>() : nullptr;
}

bool UGRGameplayAbility_BladeWaveFire::CanFireNow(double NowSeconds, float FireInterval) const
{
	return (LastFireTimeSeconds < 0.0) || ((NowSeconds - LastFireTimeSeconds) >= FireInterval);
}

void UGRGameplayAbility_BladeWaveFire::ActivateAbility(
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

	UGRAbilitySystemComponent* GRASC = GetGRASC();
	const UGRSkillAttributeSet_MeleeSkill* SkillSet = GetSkillSet();
	ACharacter* OwnerChar = Cast<ACharacter>(GetAvatarActorFromActorInfo());

	if (!GRASC || !SkillSet || !OwnerChar)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	float FireInterval = SkillSet->GetBladeWave_BaseFireInterval();
	FireInterval *= SkillSet->GetBladeWave_FireIntervalMultiplier();

	const double Now = GetWorld()->GetTimeSeconds();
	if (!CanFireNow(Now, FireInterval))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
		return;
	}
	LastFireTimeSeconds = Now;

	if (FireMontage)
	{
		PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			FireMontage,
			1.5f,
			NAME_None,
			true,
			1.0f
		);

		if (!PlayMontageTask)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		PlayMontageTask->OnCompleted.AddDynamic(this, &UGRGameplayAbility_BladeWaveFire::OnFireMontageCompleted);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &UGRGameplayAbility_BladeWaveFire::OnFireMontageCompleted);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UGRGameplayAbility_BladeWaveFire::OnFireMontageCancelled);
		PlayMontageTask->OnCancelled.AddDynamic(this, &UGRGameplayAbility_BladeWaveFire::OnFireMontageCancelled);
		PlayMontageTask->ReadyForActivation();
	}
}

void UGRGameplayAbility_BladeWaveFire::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (PlayMontageTask)
	{
		PlayMontageTask->EndTask();
		PlayMontageTask = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGRGameplayAbility_BladeWaveFire::OnFireMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGRGameplayAbility_BladeWaveFire::OnFireMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
