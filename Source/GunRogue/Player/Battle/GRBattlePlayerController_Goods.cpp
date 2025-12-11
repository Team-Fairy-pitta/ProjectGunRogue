#include "Player/GRPlayerState.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "UI/BattleHUD/GRBattleHUDWidget.h"
#include "UI/BattleHUD/SubWidgets/GRGoodsTextWidget.h"

void AGRBattlePlayerController::UpdateUICurrentMetaGoods()
{
	if (!HUDWidgetInstance)
	{
		return;
	}

	AGRPlayerState* PS = GetPlayerState<AGRPlayerState>();
	if (!PS)
	{
		return;
	}

	if (UGRGoodsTextWidget* GoodsTextWidget = HUDWidgetInstance->GetGoodsTextWidget())
	{
		GoodsTextWidget->SetGemCountText(PS->GetCurrentMetaGoods());
	}
}

void AGRBattlePlayerController::UpdateUIGold()
{
	if (!HUDWidgetInstance)
	{
		return;
	}

	AGRPlayerState* PS = GetPlayerState<AGRPlayerState>();
	if (!PS)
	{
		return;
	}

	if (UGRGoodsTextWidget* GoodsTextWidget = HUDWidgetInstance->GetGoodsTextWidget())
	{
		GoodsTextWidget->SetGoldCountText(PS->GetGold());
	}
}

