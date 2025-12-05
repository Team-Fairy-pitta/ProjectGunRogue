#include "Player/Lobby/GRLobbyPlayerController.h"
#include "UI/TitleHUD/GRLobbyHUDWidget.h"

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

	FString MapPath = GameStartMap.GetLongPackageName() + TEXT("?listen");
	GetWorld()->ServerTravel(MapPath);
}
