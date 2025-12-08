#include "GameModes/Lobby/GRLobbyGameState.h"
#include "GameFramework/PlayerState.h"
#include "Player/Lobby/GRLobbyPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

AGRLobbyGameState::AGRLobbyGameState()
{
	HostPlayerState = nullptr;
}

void AGRLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGRLobbyGameState, HostPlayerState);
	DOREPLIFETIME(AGRLobbyGameState, GuestPlayerStates);
}

void AGRLobbyGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	if (!PlayerState)
	{
		return;
	}

	// [NOTE] Dedicated Server에서는 동작하지 않음
	if (GetNetMode() == ENetMode::NM_ListenServer)
	{
		APlayerController* PlayerController = PlayerState->GetPlayerController();
		if (!PlayerController)
		{
			return;
		}

		if (PlayerController->IsLocalController())
		{
			HostPlayerState = PlayerState;
			OnRep_HostPlayerState();
		}
		else
		{
			FGuestPlayer& NewGuest = GuestPlayerStates.AddDefaulted_GetRef();
			NewGuest.GuestPlayerState = PlayerState;
			OnRep_GuestPlayerStates();
		}
	}
}

void AGRLobbyGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	if (!PlayerState)
	{
		return;
	}

	if (HostPlayerState == PlayerState)
	{
		// [NOTE] TODO: 호스트가 나갔을 때의 처리
	}
	else
	{
		int32 Index = INDEX_NONE;
		for (int Loop = 0; Loop < GuestPlayerStates.Num(); ++Loop)
		{
			if (GuestPlayerStates[Loop].GuestPlayerState == PlayerState)
			{
				Index = Loop;
				break;
			}
		}
		if (Index != INDEX_NONE)
		{
			GuestPlayerStates.RemoveAt(Index);
		}
	}
}

void AGRLobbyGameState::OnRep_HostPlayerState()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0 /*Local First Player*/);
	AGRLobbyPlayerController* LobbyPlayerController = Cast<AGRLobbyPlayerController>(PlayerController);

	if (!IsValid(LobbyPlayerController))
	{
		return;
	}

	LobbyPlayerController->UpdateHostPlayerInfo(HostPlayerState);
}

void AGRLobbyGameState::OnRep_GuestPlayerStates()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0 /*Local First Player*/);
	AGRLobbyPlayerController* LobbyPlayerController = Cast<AGRLobbyPlayerController>(PlayerController);

	if (!IsValid(LobbyPlayerController))
	{
		return;
	}

	LobbyPlayerController->UpdateGuestPlayersInfo(GuestPlayerStates);
}
