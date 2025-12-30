#include "Player/GRPlayerState.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "Character/GRCharacter.h"
#include "Character/GRPawnData.h"
#include "Character/Attachment/GRAttachmentComponent.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/GRAbilitySet.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "Weapon/GRWeaponActor.h"
#include "Weapon/GRWeaponInstance.h"
#include "Weapon/GRWeaponDefinition.h"


void AGRPlayerState::ClientRPC_BroadcastOnWeaponEquipped_Implementation(int32 SlotIndex, UGRWeaponDefinition* WeaponDefinition)
{
	OnWeaponEquipped.Broadcast(SlotIndex, WeaponDefinition);
}

void AGRPlayerState::ClientRPC_BroadcastOnWeaponDropped_Implementation(int32 SlotIndex, UGRWeaponDefinition* WeaponDefinition)
{
	OnWeaponDropped.Broadcast(SlotIndex, WeaponDefinition);
}

void AGRPlayerState::ClientRPC_BroadcastOnWeaponSwitched_Implementation(int32 OldSlotIndex, int32 NewSlotIndex)
{
	OnWeaponSwitched.Broadcast(OldSlotIndex, NewSlotIndex);
}

void AGRPlayerState::ServerRPC_EquipWeapon_Implementation(UGRWeaponDefinition* WeaponDefinition, const FGRWeaponInstance& Instance, AActor* WeaponActor)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!IsValid(WeaponDefinition))
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponDefinition is INVALID"));
		return;
	}

	if (!IsValid(AbilitySystemComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("AbilitySystemComponent is INVALID"));
		return;
	}

	int32 EmptySlot = FindEmptyWeaponSlot();

	if (EmptySlot == INDEX_NONE)
	{
		// 슬롯이 모두 찬 경우 - 현재 무기 버리고 그 슬롯에 장착
		if (CurrentWeaponSlot >= 0)
		{
			UE_LOG(LogTemp, Display, TEXT("[EquipWeapon] All slots full. Dropping weapon in slot %d"), CurrentWeaponSlot);

			UGRWeaponDefinition* DroppedWeaponDef = WeaponSlots[CurrentWeaponSlot].GetWeaponDefinition();
			FGRWeaponInstance DroppedInstance = WeaponSlots[CurrentWeaponSlot].GetWeaponInstanceCopy();

			UnequipWeaponFromSlot(CurrentWeaponSlot);
			ClientRPC_BroadcastOnWeaponDropped(CurrentWeaponSlot, DroppedWeaponDef);
			DropWeaponAtPlayerFront(DroppedWeaponDef, DroppedInstance);

			EmptySlot = CurrentWeaponSlot;
			EquipWeaponToSlot(EmptySlot, WeaponDefinition, Instance);
			SwitchToSlot(EmptySlot);
			ClientRPC_BroadcastOnWeaponEquipped(EmptySlot, WeaponDefinition);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[EquipWeapon] Weapon slots full but no current weapon"));
			return;
		}
	}
	else
	{
		// 빈 슬롯이 있는 경우
		EquipWeaponToSlot(EmptySlot, WeaponDefinition, Instance);
		ClientRPC_BroadcastOnWeaponEquipped(EmptySlot, WeaponDefinition);

		if (CurrentWeaponSlot == INDEX_NONE)
		{
			// 첫 번째 무기 - 자동 활성화
			SwitchToSlot(EmptySlot);
			ClientRPC_BroadcastOnWeaponSwitched(INDEX_NONE, EmptySlot);
			UE_LOG(LogTemp, Display, TEXT("[EquipWeapon] First weapon equipped in slot %d"), EmptySlot);
		}
		else
		{
			// [NOTE] Widget Update를 위해 무기를 잠시 스위칭해서 Attributes를 계산한다.
			WeaponSlots[CurrentWeaponSlot].DeactivateWeapon();
			WeaponSlots[EmptySlot].ActivateWeapon();
			WeaponSlots[EmptySlot].DeactivateWeapon();
			WeaponSlots[CurrentWeaponSlot].ActivateWeapon();
			UE_LOG(LogTemp, Display, TEXT("[EquipWeapon] Weapon stored in slot %d (inactive)"), EmptySlot);
		}
	}

	if (IsValid(WeaponActor))
	{
		WeaponActor->Destroy();
	}

	UE_LOG(LogTemp, Display, TEXT("[EquipWeapon] %s equipped in slot %d, Active slot: %d"),
		*WeaponDefinition->WeaponName.ToString(), EmptySlot, CurrentWeaponSlot);
}

void AGRPlayerState::ServerRPC_DropWeapon_Implementation(int32 SlotIndex)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!IsValidSlotIndex(SlotIndex))
	{
		return;
	}

	if (!WeaponSlots[SlotIndex].IsEquipped())
	{
		UE_LOG(LogTemp, Display, TEXT("No weapon in slot %d to drop"), SlotIndex);
		return;
	}

	// 무기 정보 백업
	UGRWeaponDefinition* DroppedWeaponDef = WeaponSlots[SlotIndex].GetWeaponDefinition();
	FGRWeaponInstance DroppedInstance = WeaponSlots[SlotIndex].GetWeaponInstanceCopy();

	UnequipWeaponFromSlot(SlotIndex);
	ClientRPC_BroadcastOnWeaponDropped(SlotIndex, DroppedWeaponDef);

	// 현재 활성 무기였다면 다른 무기로 전환 시도
	if (CurrentWeaponSlot == SlotIndex)
	{
		int32 OldSlot = CurrentWeaponSlot;
		CurrentWeaponSlot = INDEX_NONE;

		if (!TrySwitchToOtherWeapon(SlotIndex))
		{
			ClientRPC_BroadcastOnWeaponSwitched(OldSlot, INDEX_NONE);
			UpdateWeaponAttachToCharacter();
			ResetAmmoDisplay();
		}
	}

	DropWeaponAtPlayerFront(DroppedWeaponDef, DroppedInstance);

	UE_LOG(LogTemp, Display, TEXT("[DropWeapon] Dropped weapon from slot %d"), SlotIndex);
}

void AGRPlayerState::ServerRPC_SwitchWeapon_Implementation(int32 SlotIndex)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!IsValidSlotIndex(SlotIndex))
	{
		return;
	}

	if (!WeaponSlots[SlotIndex].IsEquipped())
	{
		UE_LOG(LogTemp, Display, TEXT("No weapon in slot %d"), SlotIndex);
		return;
	}

	if (CurrentWeaponSlot == SlotIndex)
	{
		UE_LOG(LogTemp, Display, TEXT("Already using weapon in slot %d"), SlotIndex);
		return;
	}

	// 현재 무기 비활성화
	if (CurrentWeaponSlot >= 0)
	{
		DeactivateWeaponInSlot(CurrentWeaponSlot);
	}

	int32 OldSlot = CurrentWeaponSlot;
	SwitchToSlot(SlotIndex);

	UGRWeaponDefinition* WeaponDef = WeaponSlots[SlotIndex].GetWeaponDefinition();
	if (WeaponDef)
	{
		UE_LOG(LogTemp, Display, TEXT("[SwitchWeapon] Switched to %s in slot %d"),
			*WeaponDef->WeaponName.ToString(), SlotIndex);
	}
}

void AGRPlayerState::ServerRPC_UpgradeWeapon_Implementation(int32 SlotIndex)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!WeaponSlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid weapon slot index: %d"), SlotIndex);
		return;
	}

	FGRWeaponHandle& WeaponHandle = WeaponSlots[SlotIndex];

	UGRWeaponDefinition* WeaponDefinition = WeaponHandle.GetWeaponDefinition();

	if (!IsValid(WeaponDefinition))
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponDefinition is INVALID"));
		return;
	}

	FGRWeaponInstance* WeaponInstance = WeaponHandle.GetWeaponInstanceRef();

	if (!WeaponInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponInstance is INVALID"));
		return;
	}

	if (TryCommitUpgradeWeapon(WeaponInstance))
	{
		WeaponInstance->UpgradeWeapon();
		if (SlotIndex == CurrentWeaponSlot)
		{
			WeaponInstance->ClearEffects();
			WeaponInstance->ApplyAllEffects();
		}
		else
		{
			// [NOTE] Widget Update를 위해 무기를 잠시 스위칭해서 Attributes를 계산한다.
			WeaponSlots[CurrentWeaponSlot].DeactivateWeapon();
			WeaponSlots[SlotIndex].ActivateWeapon();
			WeaponSlots[SlotIndex].DeactivateWeapon();
			WeaponSlots[CurrentWeaponSlot].ActivateWeapon();
		}
		OnRep_WeaponDataUpdata();
	}
}

bool AGRPlayerState::TryCommitUpgradeWeapon(const FGRWeaponInstance* WeaponInstance)
{
	if (!WeaponInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponInstance is INVALID"));
		return false;
	}

	int32 UpgradeCost = WeaponInstance->GetUpgradeCost();
	if (UpgradeCost <= Gold)
	{
		ApplyGoldGain(-UpgradeCost);
		return true;
	}
	else
	{
		return false;
	}
}

bool AGRPlayerState::TryCommitRerollWeapon(const FGRWeaponInstance* WeaponInstance)
{
	if (!WeaponInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponInstance is INVALID"));
		return false;
	}

	int32 RerollCost = WeaponInstance->GetRerollCost();
	if (RerollCost <= Gold)
	{
		ApplyGoldGain(-RerollCost);
		return true;
	}
	else
	{
		return false;
	}
}

void AGRPlayerState::ServerRPC_AllRerollOptionWeapon_Implementation(int32 InWeaponSlotIndex)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!WeaponSlots.IsValidIndex(InWeaponSlotIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid weapon slot index: %d"), InWeaponSlotIndex);
		return;
	}

	FGRWeaponHandle& WeaponHandle = WeaponSlots[InWeaponSlotIndex];

	UGRWeaponDefinition* WeaponDefinition = WeaponHandle.GetWeaponDefinition();

	if (!IsValid(WeaponDefinition))
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponDefinition is INVALID"));
		return;
	}

	FGRWeaponInstance* WeaponInstance = WeaponHandle.GetWeaponInstanceRef();

	if (!WeaponInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponInstance is INVALID"));
		return;
	}

	if (TryCommitRerollWeapon(WeaponInstance))
	{
		WeaponInstance->AllRerollOption();
		if (InWeaponSlotIndex == CurrentWeaponSlot)
		{
			WeaponInstance->ClearEffects();
			WeaponInstance->ApplyAllEffects();
		}
		else
		{
			// [NOTE] Widget Update를 위해 무기를 잠시 스위칭해서 Attributes를 계산한다.
			WeaponSlots[CurrentWeaponSlot].DeactivateWeapon();
			WeaponSlots[InWeaponSlotIndex].ActivateWeapon();
			WeaponSlots[InWeaponSlotIndex].DeactivateWeapon();
			WeaponSlots[CurrentWeaponSlot].ActivateWeapon();
		}
		OnRep_WeaponDataUpdata();
	}
}

void AGRPlayerState::ServerRPC_RerollOptionWeapon_Implementation(int32 InWeaponSlotIndex, int32 InOptionSlotIndex)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!WeaponSlots.IsValidIndex(InWeaponSlotIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid weapon slot index: %d"), InWeaponSlotIndex);
		return;
	}

	FGRWeaponHandle& WeaponHandle = WeaponSlots[InWeaponSlotIndex];

	UGRWeaponDefinition* WeaponDefinition = WeaponHandle.GetWeaponDefinition();

	if (!IsValid(WeaponDefinition))
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponDefinition is INVALID"));
		return;
	}

	FGRWeaponInstance* WeaponInstance = WeaponHandle.GetWeaponInstanceRef();

	if (!WeaponInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponInstance is INVALID"));
		return;
	}

	if (TryCommitRerollWeapon(WeaponInstance))
	{
		WeaponInstance->RerollOption(InOptionSlotIndex);
		if (InWeaponSlotIndex == CurrentWeaponSlot)
		{
			WeaponInstance->ClearEffects();
			WeaponInstance->ApplyAllEffects();
		}
		else
		{
			// [NOTE] Widget Update를 위해 무기를 잠시 스위칭해서 Attributes를 계산한다.
			WeaponSlots[CurrentWeaponSlot].DeactivateWeapon();
			WeaponSlots[InWeaponSlotIndex].ActivateWeapon();
			WeaponSlots[InWeaponSlotIndex].DeactivateWeapon();
			WeaponSlots[CurrentWeaponSlot].ActivateWeapon();
		}
		OnRep_WeaponDataUpdata();
	}
}

void AGRPlayerState::MulticastRPC_PlayWeaponEquipAnimMontage_Implementation()
{
	UGRWeaponDefinition* Definition = GetCurrentWeaponDefinition();
	if (!Definition)
	{
		return;
	}

	UAnimMontage* EquipAnimMontage = Definition->EquipAnimMontage;
	if (!EquipAnimMontage)
	{
		return;
	}

	AGRCharacter* GRCharacter = GetGRCharacter();
	if (!GRCharacter)
	{
		return;
	}

	GRCharacter->PlayAnimMontage(EquipAnimMontage);
}

void AGRPlayerState::ServerRPC_ResetWeaponHandles_Implementation()
{
	for (int32 SlotIndex = 0; SlotIndex < WeaponSlots.Num(); ++SlotIndex)
	{
		const FGRWeaponHandle& Handle = WeaponSlots[SlotIndex];
		if (Handle.IsEquipped())
		{
			ClientRPC_BroadcastOnWeaponEquipped(SlotIndex, Handle.GetWeaponDefinition());
		}
		if (Handle.IsActive())
		{
			SwitchToSlot(SlotIndex);
			ClientRPC_BroadcastOnWeaponSwitched(INDEX_NONE, SlotIndex);
		}
	}
}

void AGRPlayerState::UpdateWeaponAttachToCharacter()
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateWeaponAttachToCharacter requires Authority"));
		return;
	}

	AGRCharacter* GRCharacter = GetGRCharacter();
	if (!IsValid(GRCharacter))
	{
		UE_LOG(LogTemp, Warning, TEXT("GRCharacter is INVALID"));
		return;
	}

	UGRAttachmentComponent* AttachmentComponent = GRCharacter->AttachmentComponent;
	if (!AttachmentComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGRAttachmentComponent is INVALID"));
		return;
	}

	const UGRWeaponDefinition* CurrentWeaponDefinition = GetCurrentWeaponDefinition();
	if (CurrentWeaponDefinition)
	{
		// 기존에 Attach 되어있는 무기 제거
		if (CurrentWeaponAttachmentHandle.IsValid())
		{
			AttachmentComponent->RemoveCharacterAttachment(CurrentWeaponAttachmentHandle);
		}

		// 새 무기 Attach
		CurrentWeaponAttachmentHandle = AttachmentComponent->AddCharacterAttachment(CurrentWeaponDefinition->WeaponAttachment);
	}
	else
	{
		if (CurrentWeaponAttachmentHandle.IsValid())
		{
			AttachmentComponent->RemoveCharacterAttachment(CurrentWeaponAttachmentHandle);
		}
	}
}

void AGRPlayerState::TryEquipWeapon(UGRWeaponDefinition* WeaponDefinition, FGRWeaponInstance& Instance, AActor* WeaponActor)
{
	ServerRPC_EquipWeapon(WeaponDefinition, Instance, WeaponActor);
}

void AGRPlayerState::DropWeapon(int32 SlotIndex)
{
	ServerRPC_DropWeapon(SlotIndex);
}

void AGRPlayerState::DropCurrentWeapon()
{
	if (CurrentWeaponSlot >= 0)
	{
		ServerRPC_DropWeapon(CurrentWeaponSlot);
	}
}

void AGRPlayerState::SwitchWeapon(int32 SlotIndex)
{
	if (HasAuthority())
	{
		// 서버: 기존 로직 실행
		ServerRPC_SwitchWeapon(SlotIndex);
	}
	else
	{
		// 클라이언트: 즉시 로컬 예측 실행
		PredictedWeaponSlot = SlotIndex;

		// UI/애니메이션 즉시 업데이트
		if (IsValidSlotIndex(SlotIndex) && WeaponSlots[SlotIndex].IsEquipped())
		{
			OnWeaponSwitched.Broadcast(CurrentWeaponSlot, SlotIndex);
			UpdateWeaponAttachToCharacter(); // 로컬에서 즉시 무기 변경
			MulticastRPC_PlayWeaponEquipAnimMontage();
		}

		// 서버에도 요청 (검증용)
		ServerRPC_SwitchWeapon(SlotIndex);
	}
}

bool AGRPlayerState::HasWeaponInSlot(int32 SlotIndex) const
{
	if (!WeaponSlots.IsValidIndex(SlotIndex))
	{
		return false;
	}
	return WeaponSlots[SlotIndex].IsEquipped();
}

void AGRPlayerState::UpgradeWeapon(int32 SlotIndex)
{
	ServerRPC_UpgradeWeapon(SlotIndex);
}

UGRWeaponDefinition* AGRPlayerState::GetWeaponDefinitionInSlot(int32 SlotIndex) const
{
	if (!WeaponSlots.IsValidIndex(SlotIndex))
	{
		return nullptr;
	}

	return WeaponSlots[SlotIndex].GetWeaponDefinition();
}

UGRWeaponDefinition* AGRPlayerState::GetCurrentWeaponDefinition() const
{
	return GetWeaponDefinitionInSlot(CurrentWeaponSlot);
}

FGRWeaponInstance* AGRPlayerState::GetWeaponInstanceInSlot(int32 SlotIndex)
{
	if (!WeaponSlots.IsValidIndex(SlotIndex))
	{
		return nullptr;
	}

	return WeaponSlots[SlotIndex].GetWeaponInstanceRef();
}

FGRWeaponHandle* AGRPlayerState::GetActiveWeaponHandle()
{
	if (WeaponSlots.IsValidIndex(CurrentWeaponSlot))
	{
		return &WeaponSlots[CurrentWeaponSlot];
	}
	return nullptr;
}

void AGRPlayerState::OnRep_WeaponDataUpdata()
{
	if (!HasAuthority() && WeaponSlots.IsValidIndex(CurrentWeaponSlot))
	{
		FGRWeaponHandle& ActiveHandle = WeaponSlots[CurrentWeaponSlot];
		if (ActiveHandle.IsEquipped() && ActiveHandle.IsActive())
		{
			FGRWeaponInstance* WeaponInstance = ActiveHandle.GetWeaponInstanceRef();
			if (WeaponInstance && WeaponInstance->IsValid() && AbilitySystemComponent)
			{
				UGRCombatAttributeSet* CombatSet = const_cast<UGRCombatAttributeSet*>(
					AbilitySystemComponent->GetSet<UGRCombatAttributeSet>()
					);
				if (CombatSet)
				{
					CombatSet->UpdateAmmoDisplay(WeaponInstance->GetCurrentAmmo(), WeaponInstance->GetMaxAmmo());
					UE_LOG(LogTemp, Display, TEXT("[OnRep] CLIENT UI updated - Ammo: %d/%d"),
						WeaponInstance->GetCurrentAmmo(), WeaponInstance->GetMaxAmmo());
				}
			}
		}
	}

	OnWeaponDataUpdata.Broadcast();
}

void AGRPlayerState::AllRerollOptionWeapon(int32 InWeaponSlotIndex)
{
	ServerRPC_AllRerollOptionWeapon(InWeaponSlotIndex);
}

void AGRPlayerState::RerollOptionWeapon(int32 InWeaponSlotIndex, int32 InOptionSlotIndex)
{
	ServerRPC_RerollOptionWeapon(InWeaponSlotIndex, InOptionSlotIndex);
}

void AGRPlayerState::DropWeaponAtPlayerFront(UGRWeaponDefinition* WeaponDefinition, const FGRWeaponInstance& Instance)
{
	if (!HasAuthority())
	{
		return;
	}

	APawn* Pawn = GetPawn();
	if (!IsValid(Pawn) || !IsValid(WeaponDefinition))
	{
		return;
	}

	const float DropDistance = 150.0f;
	const FVector DropLocation = Pawn->GetActorLocation() +
		Pawn->GetActorForwardVector() * DropDistance;
	const FRotator DropRotation = Pawn->GetActorRotation();

	SpawnWeaponAtLocation(WeaponDefinition, Instance, DropLocation, DropRotation);
}


int32 AGRPlayerState::FindEmptyWeaponSlot() const
{
	for (int32 i = 0; i < WeaponSlots.Num(); ++i)
	{
		if (!WeaponSlots[i].IsEquipped())
		{
			return i;
		}
	}
	return INDEX_NONE;  // 빈 슬롯 없음
}

void AGRPlayerState::ActivateWeaponInSlot(int32 SlotIndex)
{
	if (!WeaponSlots.IsValidIndex(SlotIndex))
	{
		return;
	}

	// 무기 활성화 - GameplayEffect 적용
	WeaponSlots[SlotIndex].ActivateWeapon();

	UE_LOG(LogTemp, Display, TEXT("Activated weapon in slot %d"), SlotIndex);
}

void AGRPlayerState::DeactivateWeaponInSlot(int32 SlotIndex)
{
	if (!WeaponSlots.IsValidIndex(SlotIndex))
	{
		return;
	}

	// 무기 비활성화 - GameplayEffect 제거
	WeaponSlots[SlotIndex].DeactivateWeapon();

	UE_LOG(LogTemp, Display, TEXT("Deactivated weapon in slot %d"), SlotIndex);
}

void AGRPlayerState::SpawnWeaponAtLocation(
	UGRWeaponDefinition* WeaponDefinition,
	const FGRWeaponInstance& WeaponInstance,
	const FVector& Location,
	const FRotator& Rotation)
{
	if (!HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("World is INVALID"));
		return;
	}

	if (!IsValid(WeaponDefinition))
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponDefinition is INVALID"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = nullptr;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AGRWeaponActor* WeaponActor = World->SpawnActor<AGRWeaponActor>(
		AGRWeaponActor::StaticClass(),
		Location,
		Rotation,
		SpawnParams
	);

	if (IsValid(WeaponActor))
	{
		PlaceActorOnGround(WeaponActor);
		WeaponActor->InitWeapon(WeaponDefinition, WeaponInstance);
		WeaponActor->MulticastRPC_InitWeapon(WeaponDefinition, WeaponInstance);
	}
}


bool AGRPlayerState::IsValidSlotIndex(int32 SlotIndex) const
{
	if (!WeaponSlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("[Weapon] Invalid slot index: %d"), SlotIndex);
		return false;
	}
	return true;
}

void AGRPlayerState::EquipWeaponToSlot(
	int32 SlotIndex,
	UGRWeaponDefinition* WeaponDef,
	const FGRWeaponInstance& Instance)
{
	if (!IsValidSlotIndex(SlotIndex))
	{
		return;
	}

	WeaponSlots[SlotIndex].EquipWeapon(AbilitySystemComponent, WeaponDef, Instance);
	UE_LOG(LogTemp, Verbose, TEXT("[Weapon] Equipped to slot %d"), SlotIndex);
}

void AGRPlayerState::UnequipWeaponFromSlot(int32 SlotIndex)
{
	if (!IsValidSlotIndex(SlotIndex))
	{
		return;
	}

	DeactivateWeaponInSlot(SlotIndex);
	WeaponSlots[SlotIndex].UnequipWeapon();
	UE_LOG(LogTemp, Verbose, TEXT("[Weapon] Unequipped from slot %d"), SlotIndex);
}

void AGRPlayerState::SwitchToSlot(int32 NewSlotIndex)
{
	if (!IsValidSlotIndex(NewSlotIndex))
	{
		return;
	}

	PreviousWeaponSlot = CurrentWeaponSlot;
	int32 OldSlotIndex = CurrentWeaponSlot;

	// 현재 무기 비활성화
	if (CurrentWeaponSlot >= 0)
	{
		DeactivateWeaponInSlot(CurrentWeaponSlot);
	}

	OnWeaponSwitched.Broadcast(OldSlotIndex, NewSlotIndex);

	CurrentWeaponSlot = NewSlotIndex;

	// 새 무기 활성화
	ActivateWeaponInSlot(NewSlotIndex);
	UpdateWeaponAttachToCharacter();
	MulticastRPC_PlayWeaponEquipAnimMontage();
}

bool AGRPlayerState::TrySwitchToOtherWeapon(int32 ExcludeSlotIndex)
{
	for (int32 i = 0; i < WeaponSlots.Num(); ++i)
	{
		if (i != ExcludeSlotIndex && WeaponSlots[i].IsEquipped())
		{
			int32 OldSlot = CurrentWeaponSlot;
			SwitchToSlot(i);
			ClientRPC_BroadcastOnWeaponSwitched(OldSlot, i);
			UE_LOG(LogTemp, Display, TEXT("[Weapon] Auto-switched to slot %d"), i);
			return true;
		}
	}
	return false;
}

void AGRPlayerState::UpdateCurrentWeaponAmmoDisplay()
{
	if (!WeaponSlots.IsValidIndex(CurrentWeaponSlot))
	{
		return;
	}

	FGRWeaponHandle& WeaponHandle = WeaponSlots[CurrentWeaponSlot];

	if (!WeaponHandle.IsEquipped())
	{
		return;
	}

	FGRWeaponInstance* WeaponInstance = WeaponHandle.GetWeaponInstanceRef();

	if (WeaponInstance && WeaponInstance->IsValid() && AbilitySystemComponent)
	{
		UGRCombatAttributeSet* CombatSet = const_cast<UGRCombatAttributeSet*>(
			AbilitySystemComponent->GetSet<UGRCombatAttributeSet>()
			);

		if (CombatSet)
		{
			CombatSet->UpdateAmmoDisplay(
				WeaponInstance->GetCurrentAmmo(),
				WeaponInstance->GetMaxAmmo()
			);
		}
	}
}

void AGRPlayerState::ResetAmmoDisplay()
{
	if (AbilitySystemComponent)
	{
		UGRCombatAttributeSet* CombatSet =
			const_cast<UGRCombatAttributeSet*>(AbilitySystemComponent->GetSet<UGRCombatAttributeSet>());
		if (CombatSet)
		{
			CombatSet->UpdateAmmoDisplay(0, 0);
		}
	}
}

void AGRPlayerState::OnRep_CurrentWeaponSlot()
{
	OnWeaponSwitched.Broadcast(PreviousWeaponSlot, CurrentWeaponSlot);

	// 예측 검증
	if (!HasAuthority() && PredictedWeaponSlot != INDEX_NONE
		&& PredictedWeaponSlot != CurrentWeaponSlot)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SwitchWeapon] Prediction mismatch! Correcting..."));
		UpdateWeaponAttachToCharacter();
	}

	PreviousWeaponSlot = CurrentWeaponSlot;
	UpdateCurrentWeaponAmmoDisplay();
}
