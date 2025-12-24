#include "TestMeleeCharacter/TestGA_BladeWaveMode.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

UTestGA_BladeWaveMode::UTestGA_BladeWaveMode()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	Tag_BladeWaveMode = FGameplayTag::RequestGameplayTag(TEXT("State.BladeWaveMode"));

	ActivationOwnedTags.AddTag(Tag_BladeWaveMode);
}

void UTestGA_BladeWaveMode::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	//후에 애니메이션 추가

	UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, ModeDuration);
	WaitTask->OnFinish.AddDynamic(this, &UTestGA_BladeWaveMode::OnModeEnded);
	WaitTask->ReadyForActivation();
}

void UTestGA_BladeWaveMode::OnModeEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
