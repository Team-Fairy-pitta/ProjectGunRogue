// GRLevelStatusWidget.cpp


#include "UI/GRLevelStatusWidget.h"
#include "UI/GRBaseTextWidget.h"

void UGRLevelStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGRLevelStatusWidget::SetCurrentLocText(const FText& InText, const FLinearColor& InColor)
{
	if (!CurrentLocText) return;

	CurrentLocText->SetTextAndColor(
		InText,
		FSlateColor(InColor)
	);
	
}

void UGRLevelStatusWidget::SetDifficultyText(const FText& InText, const FLinearColor& InColor)
{
	if (!DifficultyText) return;

	DifficultyText->SetTextAndColor(
		InText,
		FSlateColor(InColor)
	);
}

void UGRLevelStatusWidget::SetPlayingTime(int32 InSeconds, const FLinearColor& InColor)
{
	if (!PlayingTimeText) return;
	
	int32 Minutes = InSeconds / 60;
	int32 Seconds = InSeconds % 60;

	FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	
	PlayingTimeText->SetTextAndColor(
		FText::FromString(TimeString),
		FSlateColor(InColor)
	);
}
