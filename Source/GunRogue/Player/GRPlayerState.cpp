#include "Player/GRPlayerState.h"
#include "Player/GRPlayerController.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "Character/GRCharacter.h"
#include "Character/GRPawnData.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/GRAbilitySet.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "Net/UnrealNetwork.h"
#include "Item/GRItemActor.h"
#include "Item/GRItemDefinition.h"
#include "Weapon/GRWeaponActor.h"
#include "Weapon/GRWeaponDefinition.h"

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
}

void AGRPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemHandles);
	DOREPLIFETIME(ThisClass, WeaponSlots);
	DOREPLIFETIME(ThisClass, CurrentWeaponSlot);
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

void AGRPlayerState::TryEquipWeapon(UGRWeaponDefinition* WeaponDefinition, AActor* WeaponActor)
{
	ServerRPC_EquipWeapon(WeaponDefinition, WeaponActor);
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

	AGRItemActor* ItemActor = World->SpawnActor<AGRItemActor>(AGRItemActor::StaticClass(), DropLocation, DropRotator, SpawnParam);
	if (ItemActor)
	{
		PlaceActorOnGround(ItemActor);
		ItemActor->MulticastRPC_InitItem(RemovedItemDefinition);
	}

	OnUnequipItem(RemovedItemDefinition);
}

void AGRPlayerState::ServerRPC_EquipWeapon_Implementation(UGRWeaponDefinition* WeaponDefinition, AActor* WeaponActor)
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

			// 비활성화 후 해제
			DeactivateWeaponInSlot(CurrentWeaponSlot);
			WeaponSlots[CurrentWeaponSlot].UnequipWeapon();

			OnWeaponDropped.Broadcast(CurrentWeaponSlot, DroppedWeaponDef);

			// 바닥에 무기 스폰
			DropWeaponAtPlayerFront(DroppedWeaponDef);

			EmptySlot = CurrentWeaponSlot;

			// 슬롯이 꽉 찼을 때만 새 무기로 자동 전환
			// (현재 무기를 버렸으므로 새 무기를 활성화해야 함)
			WeaponSlots[EmptySlot].EquipWeapon(AbilitySystemComponent, WeaponDefinition);
			ActivateWeaponInSlot(EmptySlot);
			CurrentWeaponSlot = EmptySlot;

			OnWeaponEquipped.Broadcast(EmptySlot, WeaponDefinition);
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
		WeaponSlots[EmptySlot].EquipWeapon(AbilitySystemComponent, WeaponDefinition);

		OnWeaponEquipped.Broadcast(EmptySlot, WeaponDefinition);

		// 첫 번째 무기라면 자동으로 활성화
		if (CurrentWeaponSlot == -1)
		{
			int32 OldSlot = CurrentWeaponSlot;

			ActivateWeaponInSlot(EmptySlot);
			CurrentWeaponSlot = EmptySlot;

			OnWeaponSwitched.Broadcast(OldSlot, CurrentWeaponSlot);

			UE_LOG(LogTemp, Display, TEXT("First weapon equipped and activated in slot %d"), EmptySlot);
		}
		else
		{
			// 이미 활성 무기가 있으면 비활성 상태로 저장만
			UE_LOG(LogTemp, Display, TEXT("Weapon stored in slot %d (inactive)"), EmptySlot);
		}
	}

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

	// 무기 해제
	DeactivateWeaponInSlot(SlotIndex);
	WeaponSlots[SlotIndex].UnequipWeapon();

	OnWeaponDropped.Broadcast(SlotIndex, DroppedWeaponDef);

	// 현재 활성 무기였다면 CurrentWeaponSlot 초기화
	if (CurrentWeaponSlot == SlotIndex)
	{
		CurrentWeaponSlot = -1;

		// 다른 슬롯에 무기가 있다면 자동으로 전환
		for (int32 i = 0; i < WeaponSlots.Num(); ++i)
		{
			if (i != SlotIndex && WeaponSlots[i].IsEquipped())
			{
				int32 OldSlot = CurrentWeaponSlot;

				ActivateWeaponInSlot(i);
				CurrentWeaponSlot = i;
				OnWeaponSwitched.Broadcast(OldSlot, CurrentWeaponSlot);
				UE_LOG(LogTemp, Display, TEXT("Auto-switched to weapon in slot %d"), i);
				break;
			}
		}
	}

	// 플레이어 앞에 무기 스폰
	DropWeaponAtPlayerFront(DroppedWeaponDef);

	UE_LOG(LogTemp, Display, TEXT("Player dropped weapon from slot %d"), SlotIndex);
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
	ActivateWeaponInSlot(SlotIndex);
	CurrentWeaponSlot = SlotIndex;

	OnWeaponSwitched.Broadcast(OldSlot, CurrentWeaponSlot);

	UGRWeaponDefinition* WeaponDef = WeaponSlots[SlotIndex].GetWeaponDefinition();
	if (WeaponDef)
	{
		UE_LOG(LogTemp, Display, TEXT("Switched to weapon: %s in slot %d"),
			*WeaponDef->WeaponName.ToString(), SlotIndex);
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

void AGRPlayerState::SpawnWeaponAtLocation(UGRWeaponDefinition* WeaponDefinition,
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
		WeaponActor->InitWeapon(WeaponDefinition);
		WeaponActor->MulticastRPC_InitWeapon(WeaponDefinition);
	}
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

void AGRPlayerState::DropWeaponAtPlayerFront(UGRWeaponDefinition* WeaponDefinition)
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

	SpawnWeaponAtLocation(WeaponDefinition, DropLocation, DropRotation);
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
