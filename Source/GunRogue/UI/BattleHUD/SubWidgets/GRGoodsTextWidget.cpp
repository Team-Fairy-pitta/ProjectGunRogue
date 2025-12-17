// GRGoodsTextWidget.cpp


#include "GRGoodsTextWidget.h"
#include "Components/TextBlock.h"

void UGRGoodsTextWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (GoldNameText)
	{
		GoldNameText->SetVisibility(ESlateVisibility::Visible);
	}
	if (GoldCountText)
	{
		GoldCountText->SetVisibility(ESlateVisibility::Visible);
	}
	if (GemNameText)
	{
		GemNameText->SetVisibility(ESlateVisibility::Visible);
	}
	if (GemCountText)
	{
		GemCountText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UGRGoodsTextWidget::SetGoodsTextInPerkHUD()
{
	if (GoldNameText)
	{
		GoldNameText->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (GoldCountText)
	{
		GoldCountText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UGRGoodsTextWidget::SetGoldCountText(int32 InCount)
{
	if (!GoldCountText)
	{
		return;
	}

	FString CountString = FString::Printf(TEXT("%d"), InCount);
	
	GoldCountText->SetText(FText::FromString(CountString));
}

void UGRGoodsTextWidget::SetGemCountText(int32 InCount)
{
	if (!GemCountText)
	{
		return;
	}

	FString CountString = FString::Printf(TEXT("%d"), InCount);
	
	GemCountText->SetText(FText::FromString(CountString));
}
