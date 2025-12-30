#include "AbilitySystem/Abilities/GRGameplayAbility_ToggleInventory.h"
#include "Character/GRCharacter.h"
#include "Player/Battle/GRBattlePlayerController.h"

UGRGameplayAbility_ToggleInventory::UGRGameplayAbility_ToggleInventory()
{
	// UI 조작은 Local에서 처리할 수 있음
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
}

void UGRGameplayAbility_ToggleInventory::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, OwnerInfo, ActivationInfo))
	{
		EndAbility(Handle, OwnerInfo, ActivationInfo, true, true);
		return;
	}

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
