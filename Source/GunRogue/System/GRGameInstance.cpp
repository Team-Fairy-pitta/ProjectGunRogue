#include "System/GRGameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Player/GRPlayerState.h"

void UGRGameInstance::CreateSession_Implementation()
{

}

void UGRGameInstance::RegistPlayerIndex(APlayerController* Player)
{
	FString ID;
	if (GetPlayerUniqueID(Player, ID))
	{
		PlayerIndexArray.Add(ID);
	}
	else
	{
		return;
	}
}

void UGRGameInstance::UnregistPlayerIndex(APlayerController* Player)
{
	FString ID;
	if (GetPlayerUniqueID(Player, ID))
	{
		PlayerIndexArray.Remove(ID);
	}
	else
	{
		return;
	}
}

int32 UGRGameInstance::GetPlayerIndex(APlayerController* Player) const
{
	FString ID;
	if (GetPlayerUniqueID(Player, ID))
	{
		return PlayerIndexArray.Find(ID);
	}
	else
	{
		return INDEX_NONE;
	}
	
}

void UGRGameInstance::SetSelectedCharacterClass(APlayerController* Player, TSubclassOf<AGRCharacter> PawnClass)
{
	FString ID;
	if (GetPlayerUniqueID(Player, ID))
	{
		SelectedCharacterMap.Add(ID, PawnClass);
	}
	else
	{
		return;
	}
}

TSubclassOf<AGRCharacter> UGRGameInstance::GetSelectedCharacterClass(APlayerController* Player) const
{
	FString ID;
	if (GetPlayerUniqueID(Player, ID))
	{
		if (SelectedCharacterMap.Contains(ID))
		{
			UE_LOG(LogTemp, Log, TEXT("GetSelectedCharacterClass: %s"), *SelectedCharacterMap[ID]->GetName());
			return SelectedCharacterMap[ID];
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}

bool UGRGameInstance::GetPlayerUniqueID(APlayerController* Player, FString& OUT PlayerID) const
{
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("Player is INVALID"));
		return false;
	}

	AGRPlayerState* PlayerState = Player->GetPlayerState<AGRPlayerState>();
	if (!IsValid(PlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerState is INVALID"));
		return false;
	}

	const FUniqueNetIdRepl& UniqueNetId = PlayerState->GetUniqueId();
	if (UniqueNetId.IsValid() && UniqueNetId->IsValid())
	{
		PlayerID = PlayerState->GetUniqueId()->ToString();
		return true;
	}
	else
	{
		return false;
	}
}
