// GRPerkTooltipWidget.cpp

#include "UI/MetaProgression/GRPerkTooltipWidget.h"
#include "Components/TextBlock.h"

void UGRPerkTooltipWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGRPerkTooltipWidget::SetPerkTooltipHeader(const FText& InText)
{
	if (!PerkTooltipHeader)
	{
		return;
	}

	PerkTooltipHeader->SetText(InText);
}

void UGRPerkTooltipWidget::SetValuePerLevelText(const FText& InText, float InValue)
{
	if (!ValuePerLevelText)
	{
		return;
	}

	FString SourceString = InText.ToString();

	bool bHasPercent = false;

	if (SourceString.Contains(TEXT("{0}%")))
	{
		bHasPercent = true;
	}

	float Value = bHasPercent ? InValue * 100.0f : InValue;

	FText FinalText = FText::FormatOrdered(InText, Value);

	ValuePerLevelText->SetText(FinalText);
}

void UGRPerkTooltipWidget::SetCurrentPerkLevelText(const FText& InText, float InValue, int32 Level)
{
	if (!CurrentPerkLevelText)
	{
		return;
	}

	FString SourceString = InText.ToString();

	bool bHasPercent = false;

	if (SourceString.Contains(TEXT("{0}%")))
	{
		bHasPercent = true;
	}

	float Value = bHasPercent ? InValue * 100.0f : InValue;

	float CurrentValue = Value * Level;

	FText FinalText = FText::FormatOrdered(InText, CurrentValue);

	CurrentPerkLevelText->SetText(FinalText);
}

void UGRPerkTooltipWidget::SetNextPerkLevelText(const FText& InText, float InValue, int32 Level, int32 MaxLevel)
{
	if (!NextPerkLevelText)
	{
		return;
	}
	
	FText FinalText;

	if (Level != MaxLevel)
	{
		FString SourceString = InText.ToString();

		bool bHasPercent = false;

		if (SourceString.Contains(TEXT("{0}%")))
		{
			bHasPercent = true;
		}

		float Value = bHasPercent ? InValue * 100.0f : InValue;

		float NextValue = Value * (Level + 1);
		
		FinalText = FText::FormatOrdered(InText, NextValue);	
	}
	else
	{
		FinalText = FText::FromString(TEXT("다음 레벨이 없습니다."));
	}
	
	NextPerkLevelText->SetText(FinalText);
}

void UGRPerkTooltipWidget::SetCostPerLevelText(int32 Level, int32 Cost, int32 MaxLevel)
{
	if (!CostPerLevelText)
	{
		return;
	}

	FString Text;
	if (Level != MaxLevel)
	{
		Text = FString::Printf(TEXT("%d등급 해방: %d Gold"), (Level + 1), Cost);
	}
	else
	{
		Text = FString::Printf(TEXT("최고 등급입니다."));
	}

	FText FinalText = FText::FromString(Text);

	CostPerLevelText->SetText(FinalText);
}


