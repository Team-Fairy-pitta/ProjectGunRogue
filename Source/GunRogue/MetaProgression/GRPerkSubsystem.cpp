// GRPerkSubsystem.cpp


#include "MetaProgression/GRPerkSubsystem.h"
#include "GRPerkSaveGame.h"
#include "PerkLevel.h"
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
			PerkLevels = LoadedPerkGame->PerkLevels;
			MetaGoods = LoadedPerkGame->MetaGoods;
			return;
		}
	}

	PerkLevels.Empty();
	MetaGoods = 0;
}

void UGRPerkSubsystem::SavePerks()
{
	FString SlotName = GetPlayerSpecificSaveSlotName();
	
	UGRPerkSaveGame* SavedPerkGame = Cast<UGRPerkSaveGame>(UGameplayStatics::CreateSaveGameObject(UGRPerkSaveGame::StaticClass()));

	SavedPerkGame->PerkLevels = PerkLevels;
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

int32 UGRPerkSubsystem::GetPerkLevel(FName PerkName) const
{
	const int32* Level = PerkLevels.Find(PerkName);
	return Level ? *Level : 0;
}

void UGRPerkSubsystem::SetPerkLevel(FName PerkName, int32 Level)
{
	PerkLevels.Add(PerkName, Level);
	SavePerks();
}

float UGRPerkSubsystem::GetPerkBonus(FName PerkName, const UDataTable* PerkTable) const
{
	if (!PerkTable)
	{
		return 0.0f;
	}

	FPerkLevel* Row = PerkTable->FindRow<FPerkLevel>(PerkName, TEXT(""));

	if (!Row)
	{
		return 0.0f;
	}

	int32 Level = GetPerkLevel(PerkName);
	return Row->ValuePerLevel * Level;
}

void UGRPerkSubsystem::AddMetaGoods(int32 Amount)
{
	MetaGoods += Amount;
	SavePerks();
}

bool UGRPerkSubsystem::TryUpgradePerk(FName PerkName, const UDataTable* PerkTable)
{
	if (!PerkTable)
	{
		return false;
	}

	FPerkLevel* Row = PerkTable->FindRow<FPerkLevel>(PerkName, TEXT(""));

	if (!Row)
	{
		return false;
	}

	int32 CurrentLevel = GetPerkLevel(PerkName);

	if (CurrentLevel >= Row->MaxLevel)
	{
		return false;
	}

	int32 Cost = (CurrentLevel) * 10;

	if (MetaGoods < Cost)
	{
		return false;
	}

	MetaGoods -= Cost;
	PerkLevels.Add(PerkName, CurrentLevel+1);

	SavePerks();

	return true;
}
