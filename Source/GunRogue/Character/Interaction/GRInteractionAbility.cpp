#include "Character/Interaction/GRInteractionAbility.h"
#include "Character/GRCharacter.h"
#include "Character/Interaction/GRInteractionComponent.h"

bool UGRInteractionAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	AGRCharacter* GRCharacter = Cast<AGRCharacter>(AvatarActor);
	return IsValid(GRCharacter);
}

void UGRInteractionAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthority(&ActivationInfo))
	{
		if (!CommitAbility(Handle, OwnerInfo, ActivationInfo))
		{
			return;
		}
		
		// 레이캐스팅
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Magenta, FString::Printf(TEXT("상호작용~")));

		EndAbility(Handle, OwnerInfo, ActivationInfo, true, false);
	}
}

void UGRInteractionAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
}
