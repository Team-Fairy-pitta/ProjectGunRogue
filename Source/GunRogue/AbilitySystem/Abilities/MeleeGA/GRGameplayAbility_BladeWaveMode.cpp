#include "AbilitySystem/Abilities/MeleeGA/GRGameplayAbility_BladeWaveMode.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Character/GRCharacter.h"
#include "Character/Attachment/GRAttachmentComponent.h"
#include "Player/GRPlayerState.h"

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

void UGRGameplayAbility_BladeWaveMode::AttachSwordToCharacter()
{
	AGRCharacter* GRCharacter = Cast<AGRCharacter>(GetAvatarActorFromActorInfo());
	if (!IsValid(GRCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BladeWaveMode] No GRCharacter"));
		return;
	}

	if (!GRCharacter->AttachmentComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BladeWaveMode] AttachmentComponent is INVALID"));
		return;
	}

	AGRPlayerState* PlayerState = GRCharacter->GetPlayerState<AGRPlayerState>();
	if (!PlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Shotgun] No PlayerState"));
		return;
	}

	FGRCharacterAttachmentHandle& CurrentWeaponAttachmentHandle = PlayerState->GetCurrentWeaponAttachmentHandle();
	if (CurrentWeaponAttachmentHandle.IsValid())
	{
		GRCharacter->AttachmentComponent->RemoveCharacterAttachment(CurrentWeaponAttachmentHandle);
	}

	SwordAttachmentHandle = GRCharacter->AttachmentComponent->AddCharacterAttachment(SwordAttachment);
}

void UGRGameplayAbility_BladeWaveMode::RollbackAttachment()
{
	AGRCharacter* GRCharacter = Cast<AGRCharacter>(GetAvatarActorFromActorInfo());
	if (!IsValid(GRCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BladeWaveMode] No GRCharacter"));
		return;
	}

	if (!GRCharacter->AttachmentComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BladeWaveMode] AttachmentComponent is INVALID"));
		return;
	}

	AGRPlayerState* PlayerState = GRCharacter->GetPlayerState<AGRPlayerState>();
	if (!PlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Shotgun] No PlayerState"));
		return;
	}

	if (SwordAttachmentHandle.IsValid())
	{
		GRCharacter->AttachmentComponent->RemoveCharacterAttachment(SwordAttachmentHandle);
	}

	PlayerState->UpdateWeaponAttachToCharacter();
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

	// 총 보이지 않게 변경, 검 장착
	AttachSwordToCharacter();
}

void UGRGameplayAbility_BladeWaveMode::OnModeEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

	// 검 해제, 총 보이도록 변경
	RollbackAttachment();
}
