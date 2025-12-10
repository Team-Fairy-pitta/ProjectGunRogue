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

#pragma region Augment
void AGRPlayerState::ServerRPC_OnAugmentSelected_Implementation(FName AugmentID)
{
	if (!HasAuthority())
	{
		return;
	}

	int32 FoundIndex = INDEX_NONE;

	for (int32 i = 0; i < OwnedAugments.Num(); i++)
	{
		if (OwnedAugments[i].AugmentID == AugmentID)
		{
			FoundIndex = i;
			break;
		}
	}

	if (FoundIndex != INDEX_NONE)
	{
		LevelUpAugment(FoundIndex);
	}
	else
	{
		AddAugment(AugmentID);
	}

	OnRep_OwnedAugments();
}

void AGRPlayerState::AddAugment(FName AugmentID)
{
	FAugmentEntry NewEntry;
	NewEntry.AugmentID = AugmentID;
	NewEntry.Level = 1;

	OwnedAugments.Add(NewEntry);
}

void AGRPlayerState::LevelUpAugment(int32 Index)
{
	if (!OwnedAugments.IsValidIndex(Index))
	{
		return;
	}

	OwnedAugments[Index].Level++;
}

int32 AGRPlayerState::GetAugmentLevel(FName AugmentID)
{
	for (const FAugmentEntry& Entry : OwnedAugments)
	{
		if (Entry.AugmentID == AugmentID)
		{
			return Entry.Level;
		}
	}

	return 0;
}

void AGRPlayerState::OnRep_OwnedAugments()
{
	for (const FAugmentEntry& Entry : OwnedAugments)
	{
		const FAugmentEntry* PrevEntry = nullptr;
		for (const FAugmentEntry& Prev : PreviousOwnedAugments)
		{
			if (Prev.AugmentID == Entry.AugmentID)
			{
				PrevEntry = &Prev;
				break;
			}
		}

		if (!PrevEntry || PrevEntry->Level != Entry.Level)
		{
			OnAugmentChanged.Broadcast(Entry.AugmentID, Entry.Level);
			UE_LOG(LogTemp, Warning, TEXT("OnRep_OwnedAugments called"));
		}
	}

	PreviousOwnedAugments = OwnedAugments;

	AGRBattlePlayerController* BattlePlayerController = GetOwner<AGRBattlePlayerController>();
	if (!IsValid(BattlePlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("GetOwner() is NOT AGRBattlePlayerController"));
		return;
	}

	BattlePlayerController->HideAugmentWidget();
}
#pragma endregion
