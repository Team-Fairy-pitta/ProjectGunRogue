#include "GameModes/Lobby/GRLobbyGameState.h"
#include "GameFramework/PlayerState.h"
#include "Player/Lobby/GRLobbyPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

AGRLobbyGameState::AGRLobbyGameState()
{
	HostPlayerState = nullptr;
}

void AGRLobbyGameState::BeginPlay()
{
	Super::BeginPlay();
}

void AGRLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGRLobbyGameState, HostPlayerState);
	DOREPLIFETIME(AGRLobbyGameState, GuestPlayerStates);
}

void AGRLobbyGameState::OnRep_ReplicatedHasBegunPlay()
{
	Super::OnRep_ReplicatedHasBegunPlay();
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

			// [NOTE] PlayerController가 준비되지 않았기 때문에, 다음 Tick에서 호출해야 함
			GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::OnRep_HostPlayerState_NextTick);
		}
		else
		{
			FGuestPlayer& NewGuest = GuestPlayerStates.AddDefaulted_GetRef();
			NewGuest.GuestPlayerState = PlayerState;

			// [NOTE] 게스트의 이름이 지정될 때 까지 기다림 (다음 Tick에서 호출해야 함)
			GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::OnRep_GuestPlayerStates_NextTick);
			GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::UpdateCanStartGame);
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

	// [NOTE] Dedicated Server에서는 동작하지 않음
	if (GetNetMode() == ENetMode::NM_ListenServer)
	{
		if (HostPlayerState == PlayerState)
		{
			// [NOTE] TODO: 호스트가 나갔을 때의 처리
		}
		else
		{
			int32 Index = FindGuestIndex(PlayerState);
			if (Index != INDEX_NONE)
			{
				GuestPlayerStates.RemoveAt(Index);

				GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::OnRep_GuestPlayerStates_NextTick);
				GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::UpdateCanStartGame);
			}
		}
	}
}

void AGRLobbyGameState::Ready(APlayerState* GuestPlayerState)
{
	if (HasAuthority())
	{
		int32 Index = FindGuestIndex(GuestPlayerState);
		if (GuestPlayerStates.IsValidIndex(Index))
		{
			GuestPlayerStates[Index].bIsReady = 1;
		}

		OnRep_GuestPlayerStates_NextTick();
		UpdateCanStartGame();
	}
}

void AGRLobbyGameState::CancelReady(APlayerState* GuestPlayerState)
{
	if (HasAuthority())
	{
		int32 Index = FindGuestIndex(GuestPlayerState);
		if (GuestPlayerStates.IsValidIndex(Index))
		{
			GuestPlayerStates[Index].bIsReady = 0;
		}

		OnRep_GuestPlayerStates_NextTick();
		UpdateCanStartGame();
	}
}

void AGRLobbyGameState::OnRep_HostPlayerState()
{
	GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::OnRep_HostPlayerState_NextTick);
}

void AGRLobbyGameState::OnRep_HostPlayerState_NextTick()
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
	GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::OnRep_GuestPlayerStates_NextTick);
}

void AGRLobbyGameState::OnRep_GuestPlayerStates_NextTick()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0 /*Local First Player*/);
	AGRLobbyPlayerController* LobbyPlayerController = Cast<AGRLobbyPlayerController>(PlayerController);

	if (!IsValid(LobbyPlayerController))
	{
		return;
	}

	LobbyPlayerController->UpdateGuestPlayersInfo(GuestPlayerStates);
}

void AGRLobbyGameState::UpdateCanStartGame()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0 /*Local First Player*/);
	AGRLobbyPlayerController* LobbyPlayerController = Cast<AGRLobbyPlayerController>(PlayerController);

	if (!IsValid(LobbyPlayerController))
	{
		return;
	}

	bool CanStartGame = IsAllPlayerReady();
	LobbyPlayerController->UpdateCanStartGame(CanStartGame);
}

int32 AGRLobbyGameState::FindGuestIndex(APlayerState* GuestPlayerState)
{
	for (int Loop = 0; Loop < GuestPlayerStates.Num(); ++Loop)
	{
		if (GuestPlayerStates[Loop].GuestPlayerState == GuestPlayerState)
		{
			return Loop;
		}
	}
	return INDEX_NONE;
}

bool AGRLobbyGameState::IsAllPlayerReady()
{
	if (GuestPlayerStates.Num() <= 0)
	{
		return true;
	}
	else
	{
		bool bAllReady = true;
		for (auto GuestPlayerState : GuestPlayerStates)
		{
			if (!GuestPlayerState.bIsReady)
			{
				bAllReady = false;
				break;
			}
		}
		return bAllReady;
	}
}
