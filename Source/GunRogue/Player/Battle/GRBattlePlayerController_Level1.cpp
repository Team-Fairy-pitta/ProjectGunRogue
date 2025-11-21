#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"
#include "GameModes/GRGameState.h"
#include "UI/Level1/GRLevel1SelectWidget.h"

void AGRBattlePlayerController::ClientRPC_ShowLevel1SelectWidget_Implementation()
{
	ShowLevel1SelectWidget();
}

void AGRBattlePlayerController::ShowLevel1SelectWidget()
{
	if (!Level1SelectWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Level1SelectWidgetInstance is INVALID"));
		return;
	}
	if (!Level1SelectWidgetInstance->IsInViewport())
	{
		Level1SelectWidgetInstance->AddToViewport();
	}

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(Level1SelectWidgetInstance->GetCachedWidget());
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

void AGRBattlePlayerController::HideLevel1SelectWidget()
{
	if (!Level1SelectWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Level1SelectWidgetInstance is INVALID"));
		return;
	}
	if (Level1SelectWidgetInstance->IsInViewport())
	{
		Level1SelectWidgetInstance->RemoveFromParent();
	}

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}
