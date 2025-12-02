#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"
#include "UI/Weapon/GRWeaponUpgrade.h"

void AGRBattlePlayerController::ClientRPC_ShowUpgradeConsoleWidget_Implementation()
{
	ShowUpgradeConsoleWidget();
}

void AGRBattlePlayerController::ShowUpgradeConsoleWidget()
{
	if (!UpgradeConsoleWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UpgradeConsoleWidgetInstance is INVALID"));
		return;
	}
	if (!UpgradeConsoleWidgetInstance->IsInViewport())
	{
		UpgradeConsoleWidgetInstance->AddToViewport();
	}

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(UpgradeConsoleWidgetInstance->GetCachedWidget());
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

void AGRBattlePlayerController::HideUpgradeConsoleWidget()
{
	if (!UpgradeConsoleWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UpgradeConsoleWidgetInstance is INVALID"));
		return;
	}
	if (UpgradeConsoleWidgetInstance->IsInViewport())
	{
		UpgradeConsoleWidgetInstance->RemoveFromParent();
	}

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}
