#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"
#include "UI/InGame/GRInGameHUDWidget.h"

void AGRBattlePlayerController::ClientRPC_ShowInGameMenuWidget_Implementation()
{
	ShowInGameMenuWidget();
}

void AGRBattlePlayerController::ShowInGameMenuWidget()
{
	if (!InGameMenuWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("InGameMenuWidgetInstance is INVALID"));
		return;
	}
	if (!InGameMenuWidgetInstance->IsInViewport())
	{
		InGameMenuWidgetInstance->AddToViewport();
	}

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(InGameMenuWidgetInstance->GetCachedWidget());
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

void AGRBattlePlayerController::HideInGameMenuWidget()
{
	if (!InGameMenuWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("InGameMenuWidgetInstance is INVALID"));
		return;
	}
	if (InGameMenuWidgetInstance->IsInViewport())
	{
		InGameMenuWidgetInstance->RemoveFromParent();
	}

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}
