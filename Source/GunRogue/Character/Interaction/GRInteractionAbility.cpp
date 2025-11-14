#include "Character/Interaction/GRInteractionAbility.h"
#include "Character/GRCharacter.h"
#include "Character/Interaction/GRInteractionComponent.h"
#include "Character/Interaction/GRInteractableActor.h"

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
			EndAbility(Handle, OwnerInfo, ActivationInfo, true, false);
			return;
		}

		if (!OwnerInfo)
		{
			UE_LOG(LogTemp, Error, TEXT("OwnerInfo is INVALID"));
			EndAbility(Handle, OwnerInfo, ActivationInfo, true, false);
			return;
		}

		if (!OwnerInfo->AvatarActor.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("OwnerInfo->AvatarActor is INVALID"));
			EndAbility(Handle, OwnerInfo, ActivationInfo, true, false);
			return;
		}

		AActor* Avartar = OwnerInfo->AvatarActor.Get();
		
		AActor* HitActor = TraceForInteractable(OwnerInfo);
		IGRInteractableActor* InteractableActor = Cast<IGRInteractableActor>(HitActor);
		if (!InteractableActor)
		{
			UE_LOG(LogTemp, Error, TEXT("HitActor is NOT IGRInteractableActor"));
			EndAbility(Handle, OwnerInfo, ActivationInfo, true, false);
			return;
		}

		InteractableActor->InteractWith(Avartar);

		EndAbility(Handle, OwnerInfo, ActivationInfo, true, false);
	}
}

AActor* UGRInteractionAbility::TraceForInteractable(const FGameplayAbilityActorInfo* OwnerInfo)
{
	if (!OwnerInfo)
	{
		UE_LOG(LogTemp, Error, TEXT("OwnerInfo is INVALID"));
		return nullptr;
	}

	if (!OwnerInfo->AvatarActor.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("OwnerInfo->AvatarActor is INVALID"));
		return nullptr;
	}

	AActor* Avartar = OwnerInfo->AvatarActor.Get();
	AGRCharacter* GRCharacter = Cast<AGRCharacter>(Avartar);
	if (!IsValid(GRCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("Avartar of ASC is NOT GRCharacter"));
		return nullptr;
	}

	UGRInteractionComponent* InteractionComponent = GRCharacter->InteractionComponent;
	if (!IsValid(InteractionComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("InteractionComponent of Avartar is INVALID"));
		return nullptr;
	}

	return InteractionComponent->TraceForInteractable();
}

void UGRInteractionAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
}
