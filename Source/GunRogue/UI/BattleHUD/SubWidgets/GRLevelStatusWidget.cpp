// GRLevelStatusWidget.cpp


#include "GRLevelStatusWidget.h"
#include "Components/TextBlock.h"
#include "GameModes/Level1/GRGameState_Level1.h"

void UGRLevelStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!GetWorld())
	{
		return;
	}

	if (GamePlayTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(GamePlayTimer);
		GamePlayTimer.Invalidate();
	}

	GetWorld()->GetTimerManager().SetTimer(GamePlayTimer, this, &ThisClass::UpdateGamePlayingTime, 0.1f, true);
}

void UGRLevelStatusWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (!GetWorld())
	{
		return;
	}

	if (GamePlayTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(GamePlayTimer);
		GamePlayTimer.Invalidate();
	}
}

void UGRLevelStatusWidget::UpdateGamePlayingTime()
{
	if (!GetWorld())
	{
		return;
	}

	AGameStateBase* GameStateBase = GetWorld()->GetGameState();
	if (!IsValid(GameStateBase))
	{
		return;
	}

	float ServerWorldTime = GameStateBase->GetServerWorldTimeSeconds();
	SetPlayingTime(ServerWorldTime);
}

void UGRLevelStatusWidget::SetCurrentLocText(const FText& InText)
{
	if (!CurrentLocText)
	{
		return;
	}

	CurrentLocText->SetText(InText);
}

void UGRLevelStatusWidget::SetPlayingTime(int32 InSeconds)
{
	if (!PlayingTimeText)
	{
		return;
	}
	
	int32 Minutes = InSeconds / 60;
	int32 Seconds = InSeconds % 60;

	FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	
	PlayingTimeText->SetText(FText::FromString(TimeString));
}
