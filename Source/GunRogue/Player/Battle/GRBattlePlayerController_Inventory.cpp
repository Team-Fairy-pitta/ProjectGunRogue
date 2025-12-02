#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"
#include "UI/Inventory/GRInventoryWidgetMain.h"

void AGRBattlePlayerController::ClientRPC_ToggleInventoryWidget_Implementation()
{
	ToggleInventoryWidget();
}

void AGRBattlePlayerController::ToggleInventoryWidget()
{
	if (InventoryWidgetInstance->IsInViewport())
	{
		HideInventoryWidget();
	}
	else
	{
		ShowInventoryWidget();
	}
}

void AGRBattlePlayerController::ShowInventoryWidget()
{
	if (!InventoryWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidgetInstance is INVALID"));
		return;
	}
	if (!InventoryWidgetInstance->IsInViewport())
	{
		InventoryWidgetInstance->AddToViewport();
		InventoryWidgetInstance->UpdateInventorySubWidgets();
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
