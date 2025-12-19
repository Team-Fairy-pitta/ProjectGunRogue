#include "Player/Battle/GRBattlePlayerController.h"

#include "UI/BattleHUD/GRBattleHUDWidget.h"
#include "UI/Boss/GRBossBarWidget.h"

void AGRBattlePlayerController::OnBossSpawned(AGRLuwoAICharacter* CurrentBoss)
{
	if (!IsLocalController() || !HUDWidgetInstance || !CurrentBoss)
	{
		return;
	}

	if (UGRBossBarWidget* BossBar = HUDWidgetInstance->GetBossHPBarWidget())
	{
		BossBar->SetBoss(CurrentBoss);
		BossBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AGRBattlePlayerController::OnBossDestroyed()
{
	if (!IsLocalController() || !HUDWidgetInstance)
	{
		return;
	}

	if (UGRBossBarWidget* BossBar =	HUDWidgetInstance->GetBossHPBarWidget())
	{
		BossBar->SetVisibility(ESlateVisibility::Collapsed);
	}
}
