#include "Player/Lobby/GRLobbyCheatManager.h"
#include "Player/Lobby/GRLobbyPlayerState.h"

void UGRLobbyCheatManager::SetMetaGoods(int32 InValue)
{
	APlayerController* PlayerController = GetPlayerController();
	if (!IsValid(PlayerController))
	{
		return;
	}

	AGRLobbyPlayerState* LobbyPlayerState = PlayerController->GetPlayerState<AGRLobbyPlayerState>();
	if (!IsValid(LobbyPlayerState))
	{
		return;
	}

	LobbyPlayerState->SetMetaGoods(InValue);
}
