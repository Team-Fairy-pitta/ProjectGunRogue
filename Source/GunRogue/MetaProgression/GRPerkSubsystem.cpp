// GRPerkSubsystem.cpp


#include "MetaProgression/GRPerkSubsystem.h"
#include "GRPerkSaveGame.h"
#include "PerkInfoRow.h"
#include "Kismet/GameplayStatics.h"
#include "MetaProgression/GRPerkStructs.h"

void UGRPerkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	PerkDataTablePath = FSoftObjectPath("/Game/GunRogue/Blueprints/MetaProgression/PerkInfoRow.PerkInfoRow");

	if (PerkDataTablePath.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("UGRPerkSubsystem: PerkSubsystem DataTable path is null"));
		return;
	}

	PerkTable = Cast<UDataTable>(PerkDataTablePath.TryLoad());
	if (!PerkTable)
	{
		UE_LOG(LogTemp, Error, TEXT("UGRPerkSubsystem: Failed to load Perk DataTable"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("UGRPerkSubsystem: Perk DataTable Loaded. RowNum = %d"), PerkTable->GetRowMap().Num());

	TArray<FPerkInfoRow*> Rows;
	PerkTable->GetAllRows(TEXT("PerkInitCheck"), Rows);

	for (auto Row : Rows)
	{
		UE_LOG(LogTemp, Log, TEXT("Loaded Perk Row: %s, MaxLevel=%d"),
			*Row->PerkID.ToString(), Row->MaxLevel);
	}
}

bool UGRPerkSubsystem::LoadPerks(const FString& PlayerID,TArray<FPerkEntry>& InPerkInfoRows, int32& InMetaGoods)
{
	FString SlotName = GetPlayerSpecificSaveSlotName(PlayerID);
	UE_LOG(LogTemp, Log, TEXT("LoadPerks : Player %s save slot = %s"), *PlayerID, *SlotName);
	
	if (UGameplayStatics::DoesSaveGameExist(SlotName, GetUserIndex()))
	{
		USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SlotName, GetUserIndex());
		UGRPerkSaveGame* LoadedPerkGame = Cast<UGRPerkSaveGame>(LoadedGame);

		if (LoadedPerkGame)
		{
			InPerkInfoRows = LoadedPerkGame->PerkInfoRows;
			InMetaGoods = LoadedPerkGame->MetaGoods;

			return true;
		}
	}

	return false;
}

void UGRPerkSubsystem::SavePerks(const FString& PlayerID, const TArray<FPerkEntry>& InPerkInfoRows, int32& InMetaGoods)
{
	FString SlotName = GetPlayerSpecificSaveSlotName(PlayerID);
	UE_LOG(LogTemp, Log, TEXT("SavePerks : Player %s save slot = %s"), *PlayerID, *SlotName);
	
	UGRPerkSaveGame* SavedPerkGame = Cast<UGRPerkSaveGame>(UGameplayStatics::CreateSaveGameObject(UGRPerkSaveGame::StaticClass()));

	SavedPerkGame->PerkInfoRows = InPerkInfoRows;
	SavedPerkGame->MetaGoods = InMetaGoods;

	UGameplayStatics::SaveGameToSlot(SavedPerkGame, SlotName, GetUserIndex());
}

FString UGRPerkSubsystem::GetPlayerSpecificSaveSlotName(const FString& PlayerId) const
{
	FString BaseSlot = TEXT("PerkSave");
	return FString::Printf(TEXT("%s_%s"), *BaseSlot, *PlayerId);
}
