// GRPerkSubsystem.cpp


#include "MetaProgression/GRPerkSubsystem.h"
#include "GRPerkSaveGame.h"
#include "PerkInfoRow.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

void UGRPerkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	//LoadPerks();
}

void UGRPerkSubsystem::LoadPerks()
{
	FString PlayerId = GetLocalPlayerUniqueId();
	FString SlotName = GetPlayerSpecificSaveSlotName(PlayerId);
	
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
	FString PlayerId = GetLocalPlayerUniqueId();
	FString SlotName = GetPlayerSpecificSaveSlotName(PlayerId);
	
	UGRPerkSaveGame* SavedPerkGame = Cast<UGRPerkSaveGame>(UGameplayStatics::CreateSaveGameObject(UGRPerkSaveGame::StaticClass()));

	SavedPerkGame->PerkInfoRows = PerkInfoRows;
	SavedPerkGame->MetaGoods = MetaGoods;

	UGameplayStatics::SaveGameToSlot(SavedPerkGame, SlotName, GetUserIndex());
}

FString UGRPerkSubsystem::GetLocalPlayerUniqueId() const
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (APlayerController* PC = GI->GetFirstLocalPlayerController())
		{
			if (PC->PlayerState)
			{
				FString ID = FString::Printf(TEXT("NetID_%d"), PC->PlayerState->GetPlayerId());
				UE_LOG(LogTemp, Warning, TEXT("[GetLocalPlayerUniqueId] Using PlayerState: %s"), *ID);
				return ID;
			}
			else if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->GetLocalPlayer()))
			{
				FString ID = FString::Printf(TEXT("ControllerID_Fallback_%d"), LocalPlayer->GetControllerId());
				UE_LOG(LogTemp, Warning, TEXT("[GetLocalPlayerUniqueId] Using LocalPlayer: %s"), *ID);
				return ID;
			}
		}
	}
	
	return TEXT("UnKnown_Player");
}

FString UGRPerkSubsystem::GetPlayerSpecificSaveSlotName(const FString& PlayerId) const
{
	FString BaseSlot = TEXT("PerkSave");
	return FString::Printf(TEXT("%s_%s"), *BaseSlot, *PlayerId);
}

int32 UGRPerkSubsystem::GetPerkLevel(FName PerkID) const
{
	const int32* Level = PerkInfoRows.Find(PerkID);
	return Level ? *Level : 0;
}

void UGRPerkSubsystem::SetPerkLevel(FName PerkID, int32 Level)
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

	int32 Level = GetPerkLevel(PerkID);
	return Row->ValuePerLevel * Level;
}

void UGRPerkSubsystem::SetMetaGoods(int32 Amount)
{
	MetaGoods = Amount;
	SavePerks();
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
	
	return true;
}

void UGRPerkSubsystem::ApplyAllPerksToASC(UAbilitySystemComponent* ASC, const UDataTable* PerkTable, TSubclassOf<UGameplayEffect> GE)
{
	if (!ASC || !PerkTable || !GE)
	{
		return;
	}

	FGameplayTag PerkRootTag = FGameplayTag::RequestGameplayTag(FName("Perk"));
	ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(PerkRootTag));

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GE, 1.f, ASC->MakeEffectContext());
	if (!SpecHandle.IsValid())
	{
		return;
	}

	SpecHandle.Data->DynamicGrantedTags.AddTag(PerkRootTag);

	UE_LOG(LogTemp, Warning, TEXT("PerkInfoRows.Num() = %d"), PerkInfoRows.Num());
	
	for (auto& Pair : PerkInfoRows)
	{
		FName PerkID = Pair.Key;
		FPerkInfoRow* Row = PerkTable->FindRow<FPerkInfoRow>(PerkID, TEXT(""));

		if (!Row) continue;

		float LevelBonus  = GetPerkBonus(PerkID, PerkTable);
		UE_LOG(LogTemp, Warning, TEXT("Setting SetByCaller %s = %f"),
	   *Row->PerkTag.ToString(), LevelBonus);

		if (LevelBonus > 0.0f)
		{
			SpecHandle.Data->SetSetByCallerMagnitude(Row->PerkTag, LevelBonus);
		}
	}
	
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}
