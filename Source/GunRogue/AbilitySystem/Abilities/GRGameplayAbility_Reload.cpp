#include "AbilitySystem/Abilities/GRGameplayAbility_Reload.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "Player/GRPlayerState.h"
#include "Weapon/GRWeaponInstance.h"
#include "AbilitySystemInterface.h"
#include "Character/GRCharacter.h"
#include "TimerManager.h"

UGRGameplayAbility_Reload::UGRGameplayAbility_Reload()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
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

	const bool bIsServer = (ASC->GetOwnerRole() == ROLE_Authority); // 🔧 수정: 서버/클라 분기

	if (bIsServer)
	{
		// 서버: PlayerState → WeaponHandle → WeaponInstance 경로로 실제 탄약 채우기
		AGRCharacter* GRCharacter = Cast<AGRCharacter>(Character);
		if (!GRCharacter)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}

		AGRPlayerState* PS = GRCharacter->GetPlayerState<AGRPlayerState>();
		if (!PS)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}

		FGRWeaponHandle* WeaponHandle = PS->GetActiveWeaponHandle();
		if (!WeaponHandle || !WeaponHandle->IsActive())
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}

		FGRWeaponInstance* WeaponInstance = WeaponHandle->GetWeaponInstanceRef();
		if (!WeaponInstance || !WeaponInstance->IsValid())
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}

		// 실제 재장전 (서버)
		WeaponInstance->Reload();

		// AttributeSet 업데이트 (서버 기준 값이 복제됨)
		CombatSet->UpdateAmmoDisplay(WeaponInstance->GetCurrentAmmo(), WeaponInstance->GetMaxAmmo());

		UE_LOG(LogTemp, Log, TEXT("[Reload] SERVER Completed: %d / %d"),
			WeaponInstance->GetCurrentAmmo(), WeaponInstance->GetMaxAmmo());
	}
	else
	{
		// 클라: CombatSet에 보이는 값 기반으로 “재장전 애니/사운드” 등 피드백만 처리
		// 실제 탄약 수는 서버에서 WeaponInstance에 적용되고, 복제 도착 시 OnRep_WeaponDataUpdata()에서 맞춰짐
		UE_LOG(LogTemp, Log, TEXT("[Reload] CLIENT predicted reload finished (real ammo on server)"));
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
