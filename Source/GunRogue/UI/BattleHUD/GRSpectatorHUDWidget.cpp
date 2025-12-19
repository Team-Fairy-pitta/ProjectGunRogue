#include "UI/BattleHUD/GRSpectatorHUDWidget.h"
#include "Components/TextBlock.h"

void UGRSpectatorHUDWidget::SetTargetPlayerName(const FString& InString)
{
	if (TargetPlayerNameText)
	{
		FText InText = FText::FromString(FString::Printf(TEXT("관전 중인 플레이어 : %s"), *InString));
		TargetPlayerNameText->SetText(InText);
	}
}

void UGRSpectatorHUDWidget::SetTargetPlayerName(const FText& InText)
{
	SetTargetPlayerName(InText.ToString());
}
