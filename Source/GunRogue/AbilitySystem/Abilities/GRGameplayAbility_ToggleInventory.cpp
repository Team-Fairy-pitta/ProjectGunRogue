#include "AbilitySystem/Abilities/GRGameplayAbility_ToggleInventory.h"
#include "Character/GRCharacter.h"
#include "Player/Battle/GRBattlePlayerController.h"

bool UGRGameplayAbility_ToggleInventory::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	AGRCharacter* GRCharacter = Cast<AGRCharacter>(AvatarActor);
	if (!IsValid(GRCharacter))
	{
		return false;
	}

	AGRBattlePlayerController* BattleController = GRCharacter->GetController<AGRBattlePlayerController>();
	if (!IsValid(BattleController))
	{
		return false;
	}

	return true;
}

void UGRGameplayAbility_ToggleInventory::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, OwnerInfo, ActivationInfo))
	{
		EndAbility(Handle, OwnerInfo, ActivationInfo, true, true);
		return;
	}

	if (HasAuthority(&ActivationInfo))
	{
		AActor* AvatarActor = OwnerInfo->AvatarActor.Get();
		AGRCharacter* GRCharacter = Cast<AGRCharacter>(AvatarActor);
		if (!IsValid(GRCharacter))
		{
			EndAbility(Handle, OwnerInfo, ActivationInfo, true, true);
			return;
		}

		AGRBattlePlayerController* BattleController = GRCharacter->GetController<AGRBattlePlayerController>();
		if (!IsValid(BattleController))
		{
			EndAbility(Handle, OwnerInfo, ActivationInfo, true, true);
			return;
		}
		BattleController->ClientRPC_ToggleInventoryWidget();

		EndAbility(Handle, OwnerInfo, ActivationInfo, true, false);
	}
	else
	{
		EndAbility(Handle, OwnerInfo, ActivationInfo, true, true);
	}
}
