#include "AbilitySystem/Abilities/GRGameplayAbility_SwitchWeapon.h"
#include "Character/GRCharacter.h"
#include "Player/GRPlayerState.h"

UGRGameplayAbility_SwitchWeapon::UGRGameplayAbility_SwitchWeapon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UGRGameplayAbility_SwitchWeapon::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// 캐릭터 유효성 검사
	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	AGRCharacter* GRCharacter = Cast<AGRCharacter>(AvatarActor);
	if (!IsValid(GRCharacter))
	{
		return false;
	}

	// PlayerState 유효성 검사
	AGRPlayerState* PlayerState = GRCharacter->GetGRPlayerState();
	if (!IsValid(PlayerState))
	{
		return false;
	}

	// 해당 슬롯에 무기가 있는지 확인
	return PlayerState->HasWeaponInSlot(TargetWeaponSlot);
}

void UGRGameplayAbility_SwitchWeapon::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* OwnerInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, OwnerInfo, ActivationInfo))
	{
		EndAbility(Handle, OwnerInfo, ActivationInfo, true, true);
		return;
	}

	AActor* AvatarActor = OwnerInfo->AvatarActor.Get();
	AGRCharacter* GRCharacter = Cast<AGRCharacter>(AvatarActor);

	if (IsValid(GRCharacter))
	{
		AGRPlayerState* PlayerState = GRCharacter->GetGRPlayerState();
		if (IsValid(PlayerState))
		{
			// 무기 전환
			PlayerState->SwitchWeapon(TargetWeaponSlot);
		}
	}

	EndAbility(Handle, OwnerInfo, ActivationInfo, true, false);
}
