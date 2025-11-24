#include "TestAnimation/GA/TestGAEquipMontage.h"
#include "TestAnimation/DA/TestWeaponAsset.h" 

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimMontage.h"
#include "TestAnimation/BP/TestGRCharacter.h"

UTestGAEquipMontage::UTestGAEquipMontage()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void UTestGAEquipMontage::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT("Equip GA Activate"));
	CommitAbility(Handle, ActorInfo, ActivationInfo);

	ATestGRCharacter* Character = Cast<ATestGRCharacter>(ActorInfo->AvatarActor.Get());

	UTestWeaponAsset* WeaponAsset = Character->CurrentWeaponAsset;
	UAnimMontage* EquipMontage = WeaponAsset->EquipMontage;

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, EquipMontage, 1.0f, NAME_None, false);

	UE_LOG(LogTemp, Warning, TEXT("EquipGA: Playing montage = %s"), *EquipMontage->GetName());


	MontageTask->OnCompleted.AddDynamic(this, &UTestGAEquipMontage::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UTestGAEquipMontage::OnMontageCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UTestGAEquipMontage::OnMontageCancelled);

	MontageTask->ReadyForActivation();
}

void UTestGAEquipMontage::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UTestGAEquipMontage::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UTestGAEquipMontage::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
