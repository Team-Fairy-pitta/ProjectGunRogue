// GRPerkSubsystem.cpp


#include "MetaProgression/GRPerkSubsystem.h"
#include "GRPerkSaveGame.h"
#include "PerkInfoRow.h"
#include "Kismet/GameplayStatics.h"

void UGRPerkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	//LoadPerks();
}

void UGRPerkSubsystem::LoadPerks()
{
	FString SlotName = GetPlayerSpecificSaveSlotName();
	
	if (UGameplayStatics::DoesSaveGameExist(SlotName, GetUserIndex()))
	{
		USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SlotName, GetUserIndex());
		UGRPerkSaveGame* LoadedPerkGame = Cast<UGRPerkSaveGame>(LoadedGame);

		if (LoadedPerkGame)
		{
			PerkInfoRows = LoadedPerkGame->PerkInfoRows;
			MetaGoods = LoadedPerkGame->MetaGoods;
		}
		else
		{
			PerkInfoRows.Empty();
			MetaGoods = 0;	
		}
	}
}

void UGRPerkSubsystem::SavePerks()
{
	FString SlotName = GetPlayerSpecificSaveSlotName();
	
	UGRPerkSaveGame* SavedPerkGame = Cast<UGRPerkSaveGame>(UGameplayStatics::CreateSaveGameObject(UGRPerkSaveGame::StaticClass()));

	SavedPerkGame->PerkInfoRows = PerkInfoRows;
	SavedPerkGame->MetaGoods = MetaGoods;

	UGameplayStatics::SaveGameToSlot(SavedPerkGame, SlotName, GetUserIndex());
}

FString UGRPerkSubsystem::GetPlayerSpecificSaveSlotName() const
{
	if (LocalPlayerUniqueId.IsEmpty())
	{
		return GetBaseSaveSlotName();
	}

	return FString::Printf(TEXT("%s_%s"), *GetBaseSaveSlotName(), *LocalPlayerUniqueId);
}

void UGRPerkSubsystem::SetLocalPlayerUniqueId(const FString& NewId)
{
	LocalPlayerUniqueId = NewId;
}

int32 UGRPerkSubsystem::GetPerkInfoRow(FName PerkID) const
{
	const int32* Level = PerkInfoRows.Find(PerkID);
	return Level ? *Level : 0;
}

void UGRPerkSubsystem::SetPerkInfoRow(FName PerkID, int32 Level)
{
	PerkInfoRows.Add(PerkID, Level);
	SavePerks();
}

float UGRPerkSubsystem::GetPerkBonus(FName PerkID, const UDataTable* PerkTable) const
{
	if (!PerkTable)
	{
		return 0.0f;
	}

	FPerkInfoRow* Row = PerkTable->FindRow<FPerkInfoRow>(PerkID, TEXT(""));

	if (!Row)
	{
		return 0.0f;
	}

	int32 Level = GetPerkInfoRow(PerkID);
	return Row->ValuePerLevel * Level;
}

void UGRPerkSubsystem::AddMetaGoods(int32 Amount)
{
	MetaGoods += Amount;
	SavePerks();
}

bool UGRPerkSubsystem::TryUpgradePerk(FName PerkID, const UDataTable* PerkTable)
{
	if (!PerkTable)
	{
		return false;
	}

	FPerkInfoRow* Row = PerkTable->FindRow<FPerkInfoRow>(PerkID, TEXT(""));

	if (!Row)
	{
		return false;
	}

	int32 CurrentLevel = GetPerkInfoRow(PerkID);

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
	SetPerkInfoRow(PerkID, CurrentLevel + 1);
	
	return true;
}
