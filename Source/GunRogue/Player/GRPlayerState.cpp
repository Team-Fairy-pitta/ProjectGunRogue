#include "Player/GRPlayerState.h"
#include "Player/GRPlayerController.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "Character/GRCharacter.h"
#include "Character/GRPawnData.h"
#include "Character/Attachment/GRAttachmentComponent.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/GRAbilitySet.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Item/GRItemActor.h"
#include "Item/GRItemDefinition.h"
#include "Weapon/GRWeaponActor.h"
#include "Weapon/GRWeaponInstance.h"
#include "Weapon/GRWeaponDefinition.h"
#include "Augment/GRAugmentStructs.h"
#include "Lobby/GRLobbyPlayerController.h"
#include "MetaProgression/GRPerkSubsystem.h"
#include "MetaProgression/GRPerkStructs.h"
#include "MetaProgression/PerkInfoRow.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"

AGRPlayerState::AGRPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UGRAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// AbilitySystem 네트워크 관련: needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);

	OnPawnSet.AddDynamic(this, &ThisClass::OnPawnSetted);

	// 무기 슬롯 초기화 (2개)
	WeaponSlots.SetNum(WeaponSlot::MaxWeaponSlots);
}

void AGRPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void AGRPlayerState::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AGRPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemHandles);
	DOREPLIFETIME(ThisClass, WeaponSlots);
	DOREPLIFETIME(ThisClass, CurrentWeaponSlot);

	DOREPLIFETIME(ThisClass, OwnedAugments);

	DOREPLIFETIME(ThisClass, CurrentMetaGoods);
}

AGRPlayerController* AGRPlayerState::GetGRPlayerController() const
{
	return Cast<AGRPlayerController>(GetOwner());
}

AGRCharacter* AGRPlayerState::GetGRCharacter() const
{
	return Cast<AGRCharacter>(GetPawn());
}

UGRAbilitySystemComponent* AGRPlayerState::GetGRAbilitySystemComponent()
{
	return Cast<UGRAbilitySystemComponent>(AbilitySystemComponent);
}

UAbilitySystemComponent* AGRPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool AGRPlayerState::HasItem(UGRItemDefinition* ItemDefinition)
{
	return ItemDefinitionSet.Contains(ItemDefinition);
}

void AGRPlayerState::TryEquipItem(UGRItemDefinition* ItemDefinition, AActor* ItemActor)
{
	ServerRPC_EquipItemActor(ItemDefinition, ItemActor);
}

void AGRPlayerState::UnequipItem(int32 ItemIndex)
{
	ServerRPC_UnequipItemActor(ItemIndex);
}

int32 AGRPlayerState::GetItemNum()
{
	return ItemHandles.Num();
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
	ServerRPC_SwitchWeapon(SlotIndex);
}

bool AGRPlayerState::HasWeaponInSlot(int32 SlotIndex) const
{
	if (!WeaponSlots.IsValidIndex(SlotIndex))
	{
		return false;
	}
	return WeaponSlots[SlotIndex].IsEquipped();
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

	WeaponInstance->UpgradeWeapon();

	OnRep_WeaponDataUpdata();
}

void AGRPlayerState::ServerRPC_EquipItemActor_Implementation(UGRItemDefinition* ItemDefinition, AActor* ItemActor)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!ItemDefinition)
	{
		return;
	}

	if (HasItem(ItemDefinition))
	{
		return;
	}

	FGRItemHandle& NewItemHandle = ItemHandles.AddDefaulted_GetRef();
	NewItemHandle.EquipItem(AbilitySystemComponent, ItemDefinition);

	ItemDefinitionSet.Add(ItemDefinition);

	AGRItemActor* GRItemActor = Cast<AGRItemActor>(ItemActor);
	if (IsValid(GRItemActor))
	{
		GRItemActor->OnPickup.Broadcast(this);
	}

	ItemActor->Destroy();
	
	OnEquipItem(ItemDefinition);
}

void AGRPlayerState::ServerRPC_UnequipItemActor_Implementation(int32 ItemIndex)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!ItemHandles.IsValidIndex(ItemIndex))
	{
		return;
	}

	APawn* Pawn = GetPawn();
	if (!IsValid(Pawn))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	if (!ItemActorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemActorClass (TSubclassOf<AGRItemActor>) is INVALID"));
		return;
	}

	FGRItemHandle& ItemHandle = ItemHandles[ItemIndex];
	ItemHandle.UnequipItem();

	UGRItemDefinition* RemovedItemDefinition = ItemHandle.ItemDefinition;
	ItemDefinitionSet.Remove(RemovedItemDefinition);

	ItemHandles.RemoveAt(ItemIndex);

	FVector DropLocation = Pawn->GetActorLocation();
	FRotator DropRotator = Pawn->GetActorRotation();
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = nullptr;

	float DropDistance = 100.0f;
	DropLocation = DropLocation + DropRotator.Vector() * DropDistance;

	AGRItemActor* ItemActor = World->SpawnActor<AGRItemActor>(ItemActorClass, DropLocation, DropRotator, SpawnParam);
	if (ItemActor)
	{
		ItemActor->MulticastRPC_InitItem(RemovedItemDefinition, EGRItemPlacement::GROUND);
	}

	OnUnequipItem(RemovedItemDefinition);
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

	// 1. 빈 슬롯 찾기
	int32 EmptySlot = FindEmptyWeaponSlot();

	if (EmptySlot == -1)
	{
		// 슬롯이 모두 찬 경우 - 현재 들고 있는 무기를 버리고 그 슬롯에 장착
		if (CurrentWeaponSlot >= 0)
		{
			UE_LOG(LogTemp, Display, TEXT("All weapon slots full. Dropping current weapon in slot %d"), CurrentWeaponSlot);

			// 현재 무기 버리기
			UGRWeaponDefinition* DroppedWeaponDef = WeaponSlots[CurrentWeaponSlot].GetWeaponDefinition();
			FGRWeaponInstance DroppedInstanceCopy = WeaponSlots[CurrentWeaponSlot].GetWeaponInstanceCopy();

			// 비활성화 후 해제
			DeactivateWeaponInSlot(CurrentWeaponSlot);
			WeaponSlots[CurrentWeaponSlot].UnequipWeapon();

			ClientRPC_BroadcastOnWeaponDropped(CurrentWeaponSlot, DroppedWeaponDef);

			// 바닥에 무기 스폰
			DropWeaponAtPlayerFront(DroppedWeaponDef, DroppedInstanceCopy);

			EmptySlot = CurrentWeaponSlot;

			// 슬롯이 꽉 찼을 때만 새 무기로 자동 전환
			// (현재 무기를 버렸으므로 새 무기를 활성화해야 함)
			WeaponSlots[EmptySlot].EquipWeapon(AbilitySystemComponent, WeaponDefinition, Instance);
			ActivateWeaponInSlot(EmptySlot);
			CurrentWeaponSlot = EmptySlot;

			ClientRPC_BroadcastOnWeaponEquipped(EmptySlot, WeaponDefinition);
			UpdateWeaponAttachToCharacter();

			// 무기 장착 애님 몽타주 재생
			MulticastRPC_PlayWeaponEquipAnimMontage();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Weapon slots full but no current weapon active"));
			return;
		}
	}
	else
	{
		// 빈 슬롯이 있는 경우 - 저장만 하고 스위칭 안 함
		WeaponSlots[EmptySlot].EquipWeapon(AbilitySystemComponent, WeaponDefinition, Instance);

		ClientRPC_BroadcastOnWeaponEquipped(EmptySlot, WeaponDefinition);

		// 첫 번째 무기라면 자동으로 활성화
		if (CurrentWeaponSlot == -1)
		{
			int32 OldSlot = CurrentWeaponSlot;
			CurrentWeaponSlot = EmptySlot;

			ClientRPC_BroadcastOnWeaponSwitched(OldSlot, CurrentWeaponSlot);
			ActivateWeaponInSlot(CurrentWeaponSlot);
			UpdateWeaponAttachToCharacter();

			// 무기 장착 애님 몽타주 재생
			MulticastRPC_PlayWeaponEquipAnimMontage();

			UE_LOG(LogTemp, Display, TEXT("First weapon equipped and activated in slot %d"), EmptySlot);
		}
		else
		{
			// 이미 활성 무기가 있으면 비활성 상태로 저장만
			UE_LOG(LogTemp, Display, TEXT("Weapon stored in slot %d (inactive)"), EmptySlot);
		}
	}

	//OnRep_WeaponDataUpdata();

	// 무기 액터 제거
	if (IsValid(WeaponActor))
	{
		WeaponActor->Destroy();
	}

	UE_LOG(LogTemp, Display, TEXT("Player equipped weapon: %s in slot %d, Current active slot: %d"),
		*WeaponDefinition->WeaponName.ToString(), EmptySlot, CurrentWeaponSlot);


}

void AGRPlayerState::ServerRPC_DropWeapon_Implementation(int32 SlotIndex)
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

	if (!WeaponSlots[SlotIndex].IsEquipped())
	{
		UE_LOG(LogTemp, Display, TEXT("No weapon in slot %d to drop"), SlotIndex);
		return;
	}

	UGRWeaponDefinition* DroppedWeaponDef = WeaponSlots[SlotIndex].GetWeaponDefinition();
	FGRWeaponInstance DroppedInstanceCopy = WeaponSlots[SlotIndex].GetWeaponInstanceCopy();

	// 무기 해제
	DeactivateWeaponInSlot(SlotIndex);
	WeaponSlots[SlotIndex].UnequipWeapon();

	ClientRPC_BroadcastOnWeaponDropped(SlotIndex, DroppedWeaponDef);

	// 현재 활성 무기였다면 CurrentWeaponSlot 초기화
	if (CurrentWeaponSlot == SlotIndex)
	{
		int32 OldSlot = CurrentWeaponSlot;
		CurrentWeaponSlot = -1;

		// 다른 슬롯에 무기가 있다면 자동으로 전환
		for (int32 i = 0; i < WeaponSlots.Num(); ++i)
		{
			if (i != SlotIndex && WeaponSlots[i].IsEquipped())
			{
				int32 NewSlot = i;
				CurrentWeaponSlot = NewSlot;

				ClientRPC_BroadcastOnWeaponSwitched(OldSlot, NewSlot);
				ActivateWeaponInSlot(NewSlot);
				UpdateWeaponAttachToCharacter();
				UE_LOG(LogTemp, Display, TEXT("Auto-switched to weapon in slot %d"), i);
				break;
			}
		}

		// 다른 슬롯에 무기가 없다면 -1
		if (CurrentWeaponSlot == -1)
		{
			ClientRPC_BroadcastOnWeaponSwitched(OldSlot, -1);
			UpdateWeaponAttachToCharacter();
		}
	}

	// 플레이어 앞에 무기 스폰
	DropWeaponAtPlayerFront(DroppedWeaponDef, DroppedInstanceCopy);

	// 무기 장착 애님 몽타주 재생 (무기를 drop하고, 다른 무기로 교체하는 경우에 재생됨)
	MulticastRPC_PlayWeaponEquipAnimMontage();

	UE_LOG(LogTemp, Display, TEXT("Player dropped weapon from slot %d"), SlotIndex);

	// 드랍 결과, 활성 무기가 없다면 Ammo/ReloadTime 0으로 리셋
	if (CurrentWeaponSlot == -1 && AbilitySystemComponent)
	{
		UGRCombatAttributeSet* CombatSet =
			const_cast<UGRCombatAttributeSet*>(AbilitySystemComponent->GetSet<UGRCombatAttributeSet>());
		if (CombatSet)
		{
			CombatSet->UpdateAmmoDisplay(0, 0);   // Ammo 0 / 0
		}
	}
}

void AGRPlayerState::ServerRPC_SwitchWeapon_Implementation(int32 SlotIndex)
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

	// 새 무기 활성화
	CurrentWeaponSlot = SlotIndex;

	ClientRPC_BroadcastOnWeaponSwitched(OldSlot, CurrentWeaponSlot);
	ActivateWeaponInSlot(CurrentWeaponSlot);
	UpdateWeaponAttachToCharacter();

	UGRWeaponDefinition* WeaponDef = WeaponSlots[SlotIndex].GetWeaponDefinition();
	if (WeaponDef)
	{
		UE_LOG(LogTemp, Display, TEXT("Switched to weapon: %s in slot %d"),
			*WeaponDef->WeaponName.ToString(), SlotIndex);
	}

	// 다른 무기로 교체할 때도 Equip 애니메이션을 재생해야 함
	MulticastRPC_PlayWeaponEquipAnimMontage();
}

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

int32 AGRPlayerState::FindEmptyWeaponSlot() const
{
	for (int32 i = 0; i < WeaponSlots.Num(); ++i)
	{
		if (!WeaponSlots[i].IsEquipped())
		{
			return i;
		}
	}
	return -1;  // 빈 슬롯 없음
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

FGRWeaponHandle* AGRPlayerState::GetActiveWeaponHandle()
{
	if (WeaponSlots.IsValidIndex(CurrentWeaponSlot))
	{
		return &WeaponSlots[CurrentWeaponSlot];
	}
	return nullptr;
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

void AGRPlayerState::OnPawnSetted(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	if (IsValid(NewPawn))
	{
		InitAbilitySystemComponent();
	}
}

void AGRPlayerState::InitAbilitySystemComponent()
{
	if (bIsAbilitySystemComponentInit)
	{
		UE_LOG(LogTemp, Error, TEXT("AbilitySystemComponent Already Init..."));
		return;
	}

	AGRCharacter* GRCharacter = GetGRCharacter();
	if (!IsValid(GRCharacter))
	{
		return;
	}

	const UGRPawnData* PawnData = GRCharacter->GetPawnData();
	if (!PawnData)
	{
		return;
	}

	if (!AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->InitAbilityActorInfo(this /*Owner*/, GRCharacter /*Avatar*/);

	for (UGRAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, &GrantedHandles);
	}

	if (OnAbilitySystemComponentInit.IsBound())
	{
		OnAbilitySystemComponentInit.Broadcast();
	}

	if (GRCharacter->IsLocallyControlled())
	{
		InitPerkFromSave();
		ServerRPC_ApplyAllPerksToASC(PerkInfoRows);
	}

	bIsAbilitySystemComponentInit = true;
}

void AGRPlayerState::OnEquipItem(UGRItemDefinition* ItemDefinition)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!ItemDefinition)
	{
		return;
	}

	AGRPlayerController* GRPlayerController = GetGRPlayerController();
	AGRBattlePlayerController* GRBattlePlayerController = Cast<AGRBattlePlayerController>(GRPlayerController);
	if (!IsValid(GRBattlePlayerController))
	{
		return;
	}

	if (!ItemDefinition->AbilitySet)
	{
		return;
	}

	for (const FGRAbilitySet_GameplayEffect& Effect : ItemDefinition->AbilitySet->GetGameplayEffects())
	{
		GRBattlePlayerController->ClientRPC_OnActiveGameplayEffectAdded(Effect.GameplayEffect);
	}
}

void AGRPlayerState::OnUnequipItem(UGRItemDefinition* ItemDefinition)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!ItemDefinition)
	{
		return;
	}

	AGRPlayerController* GRPlayerController = GetGRPlayerController();
	AGRBattlePlayerController* GRBattlePlayerController = Cast<AGRBattlePlayerController>(GRPlayerController);
	if (!IsValid(GRBattlePlayerController))
	{
		return;
	}

	if (!ItemDefinition->AbilitySet)
	{
		return;
	}

	for (const FGRAbilitySet_GameplayEffect& Effect : ItemDefinition->AbilitySet->GetGameplayEffects())
	{
		GRBattlePlayerController->ClientRPC_OnActiveGameplayEffectRemoved(Effect.GameplayEffect);
	}
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

FVector AGRPlayerState::GetGroundPointUsingLineTrace(AActor* SpawnedActor)
{
	if (!SpawnedActor)
	{
		return FVector::ZeroVector;
	}
	if (!GetWorld())
	{
		return SpawnedActor->GetActorLocation();
	}

	static const FVector FallDirection = FVector(0, 0, -1.0f);
	static const float CheckDistance = 1000.0f;
	FVector Start = SpawnedActor->GetActorLocation();
	FVector Result = Start;
	FVector End = Start + FallDirection * (CheckDistance);
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(SpawnedActor);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		Result.Z = HitResult.ImpactPoint.Z;
	}

	return Result;
}

void AGRPlayerState::PlaceActorOnGround(AActor* SpawnedActor)
{
	if (!SpawnedActor)
	{
		return;
	}

	FVector BoxOrigin;
	FVector BoxExtent;
	SpawnedActor->GetActorBounds(true, BoxOrigin, BoxExtent);
	FVector NewLocation = GetGroundPointUsingLineTrace(SpawnedActor);
	NewLocation.Z += BoxExtent.Z;
	SpawnedActor->SetActorLocation(NewLocation);
}

void AGRPlayerState::UpgradeWeapon(int32 SlotIndex)
{
	ServerRPC_UpgradeWeapon(SlotIndex);
}

void AGRPlayerState::AllRerollOptionWeapon(int32 InWeaponSlotIndex)
{
	ServerRPC_AllRerollOptionWeapon(InWeaponSlotIndex);
}

void AGRPlayerState::RerollOptionWeapon(int32 InWeaponSlotIndex, int32 InOptionSlotIndex)
{
	ServerRPC_RerollOptionWeapon(InWeaponSlotIndex, InOptionSlotIndex);
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

	WeaponInstance->RerollOption(InOptionSlotIndex);
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

	WeaponInstance->AllRerollOption();
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
