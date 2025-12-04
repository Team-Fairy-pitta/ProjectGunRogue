#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"
#include "UI/Augment/GRAugmentHUDWidget.h"

void AGRBattlePlayerController::ClientRPC_ShowAugmentWidget_Implementation()
{
	ShowAugmentWidget();
}

void AGRBattlePlayerController::ShowAugmentWidget()
{
	if (!AugmentWidgetInstance)
	{
		return;
	}

	if (!AugmentWidgetInstance->IsInViewport())
	{
		AugmentWidgetInstance->AddToViewport();
	}

	FInputModeUIOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

void AGRBattlePlayerController::RequestSelectAugment(FName AugmentID)
{
	AGRPlayerState* PS = GetPlayerState<AGRPlayerState>();
	if (!PS)
	{
		return;
	}

	PS->ServerRPC_OnAugmentSelected(AugmentID);
}
