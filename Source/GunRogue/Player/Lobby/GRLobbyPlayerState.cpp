#include "Player/Lobby/GRLobbyPlayerState.h"
#include "Player/Lobby/GRLobbyPlayerController.h"
#include "MetaProgression/GRPerkSubsystem.h"
#include "MetaProgression/GRPerkStructs.h"
#include "MetaProgression/PerkInfoRow.h"

void AGRLobbyPlayerState::InitPerkFromSave()
{
	Super::InitPerkFromSave();
	MetaGoods = CurrentMetaGoods;
}

void AGRLobbyPlayerState::SavePerkToSave()
{
	UGRPerkSubsystem* PerkSubsystem = GetGameInstance()->GetSubsystem<UGRPerkSubsystem>();
	if (!PerkSubsystem)
	{
		return;
	}

	PerkSubsystem->SavePerks(PlayerID, PerkInfoRows, MetaGoods);
}

int32 AGRLobbyPlayerState::GetPerkLevel(FName PerkID) const
{
	for (const FPerkEntry& Entry : PerkInfoRows)
	{
		if (Entry.PerkID == PerkID)
		{
			return Entry.Level;
		}
	}

	return 0;
}

void AGRLobbyPlayerState::SetPerkLevel(FName PerkID, int32 Level)
{
	for (FPerkEntry& Entry : PerkInfoRows)
	{
		if (Entry.PerkID == PerkID)
		{
			Entry.Level = Level;
			break;
		}
	}

	SavePerkToSave();
}

void AGRLobbyPlayerState::SetMetaGoods(int32 Amount)
{
	MetaGoods = Amount;

	SavePerkToSave();
}

bool AGRLobbyPlayerState::TryUpgradePerk(FName PerkID)
{
	UGRPerkSubsystem* PerkSubsystem = GetGameInstance()->GetSubsystem<UGRPerkSubsystem>();
	if (!PerkSubsystem)
	{
		return false;
	}

	UDataTable* PerkTable = PerkSubsystem->GetPerkTable();
	if (!PerkTable)
	{
		return false;
	}

	FPerkInfoRow* Row = PerkTable->FindRow<FPerkInfoRow>(PerkID, TEXT(""));
	if (!Row)
	{
		return false;
	}

	int32 CurrentLevel = GetPerkLevel(PerkID);
	if (CurrentLevel >= Row->MaxLevel)
	{
		return false;
	}

	int32 Cost = (CurrentLevel + 1) * Row->CostPerLevel;
	if (MetaGoods < Cost)
	{
		return false;
	}

	MetaGoods -= Cost;
	SetPerkLevel(PerkID, CurrentLevel + 1);

	UE_LOG(LogTemp, Log, TEXT("TryUpgradePerk SUCCESS: Perk upgraded. PerkID: %s, NewLevel: %d, MetaGoods Left: %d"),
		*PerkID.ToString(), CurrentLevel + 1, GetMetaGoods());

	return true;
}

void AGRLobbyPlayerState::OnPawnSetted(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	if (NewPawn && NewPawn->IsLocallyControlled())
	{
		InitPerkFromSave();
		SavePerkToSave();
	}
}
