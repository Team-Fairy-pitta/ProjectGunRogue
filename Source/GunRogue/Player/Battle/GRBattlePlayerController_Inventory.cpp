#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"
#include "UI/Inventory/GRInventoryWidget.h"

void AGRBattlePlayerController::ClientRPC_ShowInventoryWidget_Implementation()
{
	ShowInventoryWidget();
}

void AGRBattlePlayerController::ShowInventoryWidget()
{
	if (!InventoryWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UpgradeConsoleWidgetInstance is INVALID"));
		return;
	}
	if (!InventoryWidgetInstance->IsInViewport())
	{
		InventoryWidgetInstance->AddToViewport();
	}

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(InventoryWidgetInstance->GetCachedWidget());
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

void AGRBattlePlayerController::HideInventoryWidget()
{
	if (!InventoryWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidgetInstance is INVALID"));
		return;
	}
	if (InventoryWidgetInstance->IsInViewport())
	{
		InventoryWidgetInstance->RemoveFromParent();
	}

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}
