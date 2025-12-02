#include "AbilitySystem/Abilities/GRGameplayAbility_Reload.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "AbilitySystemInterface.h"
#include "Character/GRCharacter.h"
#include "TimerManager.h"

UGRGameplayAbility_Reload::UGRGameplayAbility_Reload()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UGRGameplayAbility_Reload::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// CombatAttributeSet 확인
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return false;
	}

	const UGRCombatAttributeSet* CombatSet = ActorInfo->AbilitySystemComponent->GetSet<UGRCombatAttributeSet>();
	if (!CombatSet)
	{
		return false;
	}

	// 재장전 가능 여부 체크 (탄약이 꽉 차있으면 불가)
	if (!CombatSet->CheckCanReload())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Reload] Cannot reload - Already full: %.0f / %.0f"),
			CombatSet->GetCurrentAmmo(), CombatSet->GetMaxAmmo());
		return false;
	}

	return true;
}

void UGRGameplayAbility_Reload::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Character);
	if (!ASI)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const UGRCombatAttributeSet* CombatSet = ASC->GetSet<UGRCombatAttributeSet>();
	if (!CombatSet)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const float ReloadDuration = CombatSet->GetReloadTime();

	UE_LOG(LogTemp, Log, TEXT("[Reload] Started - Duration: %.2f seconds"), ReloadDuration);

	// TODO: 여기에 재장전 애니메이션 재생 (Montage)
	// PlayMontageAndWait(ReloadMontage);

	// 재장전 타이머 시작
	TWeakObjectPtr<UGRGameplayAbility_Reload> WeakThis(this);
	GetWorld()->GetTimerManager().SetTimer(
		ReloadTimerHandle,
		[WeakThis]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->PerformReload();
			}
		},
		ReloadDuration,
		false
	);
}

void UGRGameplayAbility_Reload::PerformReload()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Character);
	if (!ASI)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	UGRCombatAttributeSet* CombatSet = const_cast<UGRCombatAttributeSet*>(
		ASC->GetSet<UGRCombatAttributeSet>()
		);

	if (CombatSet)
	{
		// 재장전 실행
		CombatSet->ReloadAmmo(ASC);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGRGameplayAbility_Reload::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// 타이머 정리
	if (GetWorld() && ReloadTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	}

	if (bWasCancelled)
	{
		UE_LOG(LogTemp, Log, TEXT("[Reload] Cancelled"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[Reload] Completed"));
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
