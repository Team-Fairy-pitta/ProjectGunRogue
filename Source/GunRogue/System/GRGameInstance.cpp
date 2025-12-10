#include "System/GRGameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Player/GRPlayerState.h"

void UGRGameInstance::CreateSession_Implementation()
{

}

void UGRGameInstance::SetSelectedCharacterClass(APlayerController* Player, TSubclassOf<AGRCharacter> PawnClass)
{
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("Player is INVALID"));
		return;
	}

	AGRPlayerState* PlayerState = Player->GetPlayerState<AGRPlayerState>();
	if (!IsValid(PlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerState is INVALID"));
		return;
	}

	FUniqueNetIdRepl ID = PlayerState->GetUniqueId();
	SelectedCharacterMap.Add(ID, PawnClass);
}

TSubclassOf<AGRCharacter> UGRGameInstance::GetSelectedCharacterClass(APlayerController* Player) const
{
	if (!IsValid(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("Player is INVALID"));
		return nullptr;
	}

	AGRPlayerState* PlayerState = Player->GetPlayerState<AGRPlayerState>();
	if (!IsValid(PlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerState is INVALID"));
		return nullptr;
	}

	FUniqueNetIdRepl ID = PlayerState->GetUniqueId();
	if (SelectedCharacterMap.Contains(ID))
	{
		return SelectedCharacterMap[ID];
	}
	else
	{
		return nullptr;
	}
}
