#include "Player/Lobby/GRLobbyPlayerController.h"
#include "UI/TitleHUD/GRLobbyHUDWidget.h"
#include "UI/TitleHUD/SubWidgets/GRLobbyPlayerListWidget.h"
#include "GameModes/Lobby/GRLobbyGameState.h"
#include "GameFramework/PlayerState.h"

AGRLobbyPlayerController::AGRLobbyPlayerController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGRLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		CreateWidgets();
		ShowLobbyWidget();
	}
}

void AGRLobbyPlayerController::CreateWidgets()
{
	if (!LobbyWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyWidgetClass (TSubclassOf<UGRLobbyHUDWidget>) is INVALID"));
		return;
	}

	LobbyWidgetInstance = CreateWidget<UGRLobbyHUDWidget>(this, LobbyWidgetClass);
	if (!LobbyWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("CANNOT Create UGRLobbyHUDWidget Widgets"));
		return;
	}

	if (!LoadingWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("LoadingWidgetClass (TSubclassOf<UUserWidget>) is INVALID"));
		return;
	}

	LoadingWidgetInstance = CreateWidget<UUserWidget>(this, LoadingWidgetClass);
	if (!LoadingWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("CANNOT Create LoadingWidgetInstance Widgets"));
		return;
	}
}

void AGRLobbyPlayerController::ShowLobbyWidget()
{
	if (!LobbyWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyWidgetInstance is INVALID"));
		return;
	}
	if (!LobbyWidgetInstance->IsInViewport())
	{
		LobbyWidgetInstance->AddToViewport();
	}

	FInputModeUIOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

void AGRLobbyPlayerController::HideLobbyWidget()
{
	if (!LobbyWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyWidgetInstance is INVALID"));
		return;
	}
	if (LobbyWidgetInstance->IsInViewport())
	{
		LobbyWidgetInstance->RemoveFromParent();
	}

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}

void AGRLobbyPlayerController::ShowLoadingWidget()
{
	if (!LoadingWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("LoadingWidgetInstance is INVALID"));
		return;
	}
	if (!LoadingWidgetInstance->IsInViewport())
	{
		LoadingWidgetInstance->AddToViewport();
	}

	FInputModeUIOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

void AGRLobbyPlayerController::HideLoadingWidget()
{
	if (!LoadingWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("LoadingWidgetInstance is INVALID"));
		return;
	}
	if (LoadingWidgetInstance->IsInViewport())
	{
		LoadingWidgetInstance->RemoveFromParent();
	}

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}

void AGRLobbyPlayerController::StartGame()
{
	if (GetNetMode() == ENetMode::NM_ListenServer)
	{
		ServerRPC_StartGame();
	}
	else if (GetNetMode() == ENetMode::NM_Client)
	{
		// [NOTE] 고려하지 않음
	}
	else if (GetNetMode() == ENetMode::NM_Standalone)
	{
		// [NOTE] 고려하지 않음
	}
	else
	{
		// [NOTE] 고려하지 않음
	}
}

void AGRLobbyPlayerController::UpdateHostPlayerInfo(APlayerState* HostPlayer)
{
	if (!LobbyWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyWidgetInstance is INVALID"));
		return;
	}

	UGRLobbyPlayerListWidget* PlayerListWidget = LobbyWidgetInstance->GetLobbyPlayerListWidget();
	if (!PlayerListWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerListWidget is INVALID"));
		return;
	}

	PlayerListWidget->UpdateHostPlayerInfo(HostPlayer);
}

void AGRLobbyPlayerController::UpdateGuestPlayersInfo(TArray<FGuestPlayer>& GuestPlayers)
{
	if (!LobbyWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyWidgetInstance is INVALID"));
		return;
	}

	UGRLobbyPlayerListWidget* PlayerListWidget = LobbyWidgetInstance->GetLobbyPlayerListWidget();
	if (!PlayerListWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerListWidget is INVALID"));
		return;
	}

	PlayerListWidget->UpdateGuestPlayersInfo(GuestPlayers);
}

void AGRLobbyPlayerController::ClientRPC_ShowLoadingWidget_Implementation()
{
	ShowLoadingWidget();
}

void AGRLobbyPlayerController::UpdateCanStartGame(bool bCanStart)
{
	if (!LobbyWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyWidgetInstance is INVALID"));
		return;
	}

	if (bCanStart)
	{
		LobbyWidgetInstance->EnableStartButton();
	}
	else
	{
		LobbyWidgetInstance->DisableStartButton();
	}
}

void AGRLobbyPlayerController::ServerRPC_StartGame_Implementation()
{
	if (!HasAuthority())
	{
		return;
	}

	if (!GetWorld())
	{
		return;
	}

	if (GameStartMap.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("GameStartMap is NULL"));
		return;
	}

	AGRLobbyGameState* LobbyGameState = GetWorld()->GetGameState<AGRLobbyGameState>();
	if (!IsValid(LobbyGameState))
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyGameState is INVALID"));
		return;
	}

	for (APlayerState* State : LobbyGameState->PlayerArray)
	{
		if (IsValid(State))
		{
			AGRLobbyPlayerController* Controller = Cast<AGRLobbyPlayerController>(State->GetOwner());
			if (IsValid(Controller))
			{
				Controller->ClientRPC_ShowLoadingWidget();
			}
		}
	}

	FString MapPath = GameStartMap.GetLongPackageName() + TEXT("?listen");
	GetWorld()->ServerTravel(MapPath);
}

void AGRLobbyPlayerController::ServerRPC_Ready_Implementation()
{
	if (!HasAuthority())
	{
		return;
	}
	if (!GetWorld())
	{
		return;
	}

	AGRLobbyGameState* LobbyGameState = GetWorld()->GetGameState<AGRLobbyGameState>();
	if (!IsValid(LobbyGameState))
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyGameState is INVALID"));
		return;
	}
	LobbyGameState->Ready(GetPlayerState<APlayerState>());

	ClientRPC_OnConfirmReady();
}

void AGRLobbyPlayerController::ServerRPC_CancelReady_Implementation()
{
	if (!HasAuthority())
	{
		return;
	}
	if (!GetWorld())
	{
		return;
	}

	AGRLobbyGameState* LobbyGameState = GetWorld()->GetGameState<AGRLobbyGameState>();
	if (!IsValid(LobbyGameState))
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyGameState is INVALID"));
		return;
	}
	LobbyGameState->CancelReady(GetPlayerState<APlayerState>());

	ClientRPC_OnConfirmCancelReady();
}

void AGRLobbyPlayerController::ClientRPC_OnConfirmReady_Implementation()
{
	if (!LobbyWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyWidgetInstance is INVALID"));
		return;
	}

	LobbyWidgetInstance->EnableReadyButton();
}

void AGRLobbyPlayerController::ClientRPC_OnConfirmCancelReady_Implementation()
{
	if (!LobbyWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyWidgetInstance is INVALID"));
		return;
	}
	LobbyWidgetInstance->EnableButtons();
	LobbyWidgetInstance->EnableReadyButton();
}
