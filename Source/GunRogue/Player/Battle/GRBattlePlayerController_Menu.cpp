#include "Player/Battle/GRBattlePlayerController.h"

#include "UI/InGame/GRInGameHUDWidget.h"

void AGRBattlePlayerController::ClientRPC_ShowMenuWidget_Implementation()
{
	ShowMenuWidget();
}

void AGRBattlePlayerController::ShowMenuWidget()
{
	if (!MenuWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("MenuWidgetInstance is INVALID"));
		return;
	}
	if (!MenuWidgetInstance->IsInViewport())
	{
		MenuWidgetInstance->AddToViewport();
	}

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(MenuWidgetInstance->GetCachedWidget());
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

void AGRBattlePlayerController::HideMenuWidget()
{
	if (!MenuWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("MenuWidgetInstance is INVALID"));
		return;
	}
	if (MenuWidgetInstance->IsInViewport())
	{
		MenuWidgetInstance->RemoveFromParent();
	}

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}
