// GRLevelStatusWidget.cpp


#include "GRLevelStatusWidget.h"
#include "Components/TextBlock.h"

void UGRLevelStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGRLevelStatusWidget::SetCurrentLocText(const FText& InText)
{
	if (!CurrentLocText)
	{
		return;
	}

	CurrentLocText->SetText(InText);
}

void UGRLevelStatusWidget::SetDifficultyText(const FText& InText)
{
	if (!DifficultyText)
	{
		return;
	}

	DifficultyText->SetText(InText);
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
