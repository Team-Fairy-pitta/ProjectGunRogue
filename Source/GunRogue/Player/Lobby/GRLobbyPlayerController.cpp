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

void AGRLobbyPlayerController::ShowESCMenuWidget()
{
}

void AGRLobbyPlayerController::HideESCMenuWidget()
{
}
