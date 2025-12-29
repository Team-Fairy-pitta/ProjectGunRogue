#include "AbilitySystem/Abilities/MeleeGA/GRGameplayAbility_BladeWaveMode.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Character/GRCharacter.h"

UGRGameplayAbility_BladeWaveMode::UGRGameplayAbility_BladeWaveMode()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	Tag_BladeWaveMode = FGameplayTag::RequestGameplayTag(TEXT("State.BladeWaveMode"));

	ActivationOwnedTags.AddTag(Tag_BladeWaveMode);
}

bool UGRGameplayAbility_BladeWaveMode::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(
		Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	AGRCharacter* GRChar = Cast<AGRCharacter>(ActorInfo->AvatarActor.Get());
	if (!GRChar)
	{
		return false;
	}

	USkeletalMeshComponent* WeaponMesh = GRChar->GetEquippedWeaponMesh();

	if (!WeaponMesh || !WeaponMesh->GetSkeletalMeshAsset())
	{
		return false;
	}

	return true;
}

void UGRGameplayAbility_BladeWaveMode::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	//TODO : 애니메이션 추가

	UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, ModeDuration);
	WaitTask->OnFinish.AddDynamic(this, &UGRGameplayAbility_BladeWaveMode::OnModeEnded);
	WaitTask->ReadyForActivation();
}

void UGRGameplayAbility_BladeWaveMode::OnModeEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
