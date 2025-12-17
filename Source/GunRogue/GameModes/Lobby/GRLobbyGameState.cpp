#include "GameModes/Lobby/GRLobbyGameState.h"
#include "Player/Lobby/GRLobbyPlayerController.h"
#include "Player/Lobby/GRLobbyPlayerState.h"
#include "System/GRGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

AGRLobbyGameState::AGRLobbyGameState()
{
}

void AGRLobbyGameState::BeginPlay()
{
	Super::BeginPlay();
}

void AGRLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGRLobbyGameState, HostPlayer);
	DOREPLIFETIME(AGRLobbyGameState, GuestPlayers);
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

	if (!GetWorld())
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

		AGRLobbyPlayerState* LobbyPlayerState = Cast<AGRLobbyPlayerState>(PlayerState);
		if (!LobbyPlayerState)
		{
			return;
		}

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &ThisClass::RegistPlayerIndex, PlayerController);
		GetWorldTimerManager().SetTimerForNextTick(TimerDelegate);

		if (PlayerController->IsLocalController())
		{
			HostPlayer.PlayerState = LobbyPlayerState;

			// [NOTE] PlayerController가 준비되지 않았기 때문에, 다음 Tick에서 호출해야 함
			GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::OnRep_HostPlayer_NextTick);
		}
		else
		{
			FGuestPlayer& NewGuest = GuestPlayers.AddDefaulted_GetRef();
			NewGuest.PlayerState = LobbyPlayerState;

			// [NOTE] 게스트의 이름이 지정될 때 까지 기다림 (다음 Tick에서 호출해야 함)
			GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::OnRep_GuestPlayers_NextTick);
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
		APlayerController* PlayerController = PlayerState->GetPlayerController();
		if (!PlayerController)
		{
			return;
		}

		UGRGameInstance* GRGameInstance = GetWorld()->GetGameInstance<UGRGameInstance>();
		if (!GRGameInstance)
		{
			return;
		}

		GRGameInstance->UnregistPlayerIndex(PlayerController);

		if (HostPlayer.PlayerState == PlayerState)
		{
			// [NOTE] TODO: 호스트가 나갔을 때의 처리
		}
		else
		{
			int32 Index = FindGuestIndex(PlayerState);
			if (Index != INDEX_NONE)
			{
				GuestPlayers.RemoveAt(Index);

				GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::OnRep_GuestPlayers_NextTick);
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
		if (GuestPlayers.IsValidIndex(Index))
		{
			GuestPlayers[Index].bIsReady = 1;
		}

		OnRep_GuestPlayers_NextTick();
		UpdateCanStartGame();
	}
}

void AGRLobbyGameState::CancelReady(APlayerState* GuestPlayerState)
{
	if (HasAuthority())
	{
		int32 Index = FindGuestIndex(GuestPlayerState);
		if (GuestPlayers.IsValidIndex(Index))
		{
			GuestPlayers[Index].bIsReady = 0;
		}

		OnRep_GuestPlayers_NextTick();
		UpdateCanStartGame();
	}
}

void AGRLobbyGameState::SelectCharacterClass(APlayerState* PlayerState, TSubclassOf<AGRCharacter> SelectedCharacterClass)
{
	if (HasAuthority())
	{
		if (PlayerState == HostPlayer.PlayerState)
		{
			HostPlayer.SelectedCharacterClass = SelectedCharacterClass;
			OnRep_HostPlayer_NextTick();
		}
		else
		{
			int32 Index = FindGuestIndex(PlayerState);
			if (GuestPlayers.IsValidIndex(Index))
			{
				GuestPlayers[Index].SelectedCharacterClass = SelectedCharacterClass;
				OnRep_GuestPlayers_NextTick();
			}
		}
	}
}

void AGRLobbyGameState::OnRep_HostPlayer()
{
	GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::OnRep_HostPlayer_NextTick);
}

void AGRLobbyGameState::OnRep_HostPlayer_NextTick()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0 /*Local First Player*/);
	AGRLobbyPlayerController* LobbyPlayerController = Cast<AGRLobbyPlayerController>(PlayerController);

	if (!IsValid(LobbyPlayerController))
	{
		return;
	}

	LobbyPlayerController->UpdateHostPlayerInfo(HostPlayer);
}

void AGRLobbyGameState::OnRep_GuestPlayers()
{
	GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::OnRep_GuestPlayers_NextTick);
}

void AGRLobbyGameState::OnRep_GuestPlayers_NextTick()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0 /*Local First Player*/);
	AGRLobbyPlayerController* LobbyPlayerController = Cast<AGRLobbyPlayerController>(PlayerController);

	if (!IsValid(LobbyPlayerController))
	{
		return;
	}

	LobbyPlayerController->UpdateGuestPlayersInfo(GuestPlayers);
}

void AGRLobbyGameState::UpdateCanStartGame()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0 /*Local First Player*/);
	AGRLobbyPlayerController* LobbyPlayerController = Cast<AGRLobbyPlayerController>(PlayerController);

	if (!IsValid(LobbyPlayerController))
	{
		return;
	}

	bool bHostPlayerSelectCharacter = HostPlayer.SelectedCharacterClass != nullptr;
	bool bIsAllAready = IsAllPlayerReady();
	bool CanStartGame = bHostPlayerSelectCharacter && bIsAllAready;
	LobbyPlayerController->UpdateCanStartGame(CanStartGame);
}

int32 AGRLobbyGameState::FindGuestIndex(APlayerState* GuestPlayerState)
{
	for (int Loop = 0; Loop < GuestPlayers.Num(); ++Loop)
	{
		if (GuestPlayers[Loop].PlayerState == GuestPlayerState)
		{
			return Loop;
		}
	}
	return INDEX_NONE;
}

bool AGRLobbyGameState::IsAllPlayerReady()
{
	if (GuestPlayers.Num() <= 0)
	{
		return true;
	}
	else
	{
		bool bAllReady = true;
		for (auto GuestPlayerState : GuestPlayers)
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

void AGRLobbyGameState::RegistPlayerIndex(APlayerController* PlayerController)
{
	UGRGameInstance* GRGameInstance = GetWorld()->GetGameInstance<UGRGameInstance>();
	if (!GRGameInstance)
	{
		return;
	}
	GRGameInstance->RegistPlayerIndex(PlayerController);

}
