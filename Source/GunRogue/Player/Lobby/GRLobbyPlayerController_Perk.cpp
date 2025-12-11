#include "Player/Lobby/GRLobbyPlayerController.h"
#include "UI/MetaProgression/GRPerkHUDWidget.h"
#include "Player/GRPlayerState.h"

void AGRLobbyPlayerController::ClientRPC_ShowPerkWidget_Implementation()
{
	ShowPerkWidget();
}

void AGRLobbyPlayerController::ShowPerkWidget()
{
	if (!PerkWidgetInstance)
	{
		return;
	}

	if (!PerkWidgetInstance->IsInViewport())
	{
		PerkWidgetInstance->AddToViewport();
	}

	FInputModeGameAndUI Mode;
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

void AGRLobbyPlayerController::HidePerkWidget()
{
	if (!PerkWidgetInstance)
	{
		return;
	}

	if (PerkWidgetInstance->IsInViewport())
	{
		PerkWidgetInstance->RemoveFromParent();
	}

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}





