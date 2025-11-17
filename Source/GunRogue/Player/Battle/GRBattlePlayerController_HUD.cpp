#include "Player/Battle/GRBattlePlayerController.h"
#include "UI/BattleHUD/GRBattleHUDWidget.h"
#include "UI/BattleHUD/SubWidgets/GRPlayerStatusWidget.h"

void AGRBattlePlayerController::UpdatePlayerHealth(float Value)
{
	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetInstance (UGRBattleHUDWidget) is INVALID"));
		return;
	}

	UGRPlayerStatusWidget* PlayerStatusWidget = HUDWidgetInstance->GetPlayerStatusWidget();
	if (!PlayerStatusWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerStatusWidget (UGRPlayerStatusWidget) is INVALID"));
		return;
	}

	PlayerStatusWidget->SetPlayerHealth(Value);
}

void AGRBattlePlayerController::UpdatePlayerMaxHealth(float Value)
{
	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetInstance (UGRBattleHUDWidget) is INVALID"));
		return;
	}

	UGRPlayerStatusWidget* PlayerStatusWidget = HUDWidgetInstance->GetPlayerStatusWidget();
	if (!PlayerStatusWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerStatusWidget (UGRPlayerStatusWidget) is INVALID"));
		return;
	}

	PlayerStatusWidget->SetPlayerMaxHealth(Value);
}

void AGRBattlePlayerController::UpdatePlayerShield(float Value)
{
	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetInstance (UGRBattleHUDWidget) is INVALID"));
		return;
	}

	UGRPlayerStatusWidget* PlayerStatusWidget = HUDWidgetInstance->GetPlayerStatusWidget();
	if (!PlayerStatusWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerStatusWidget (UGRPlayerStatusWidget) is INVALID"));
		return;
	}

	PlayerStatusWidget->SetPlayerShield(Value);
}

void AGRBattlePlayerController::UpdatePlayerMaxShield(float Value)
{
	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetInstance (UGRBattleHUDWidget) is INVALID"));
		return;
	}

	UGRPlayerStatusWidget* PlayerStatusWidget = HUDWidgetInstance->GetPlayerStatusWidget();
	if (!PlayerStatusWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerStatusWidget (UGRPlayerStatusWidget) is INVALID"));
		return;
	}

	PlayerStatusWidget->SetPlayerMaxShield(Value);
}
