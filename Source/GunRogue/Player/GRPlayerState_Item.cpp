#include "Player/GRPlayerState.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "Character/GRCharacter.h"
#include "Character/GRPawnData.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/GRAbilitySet.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "Item/GRItemActor.h"
#include "Item/GRItemDefinition.h"

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
