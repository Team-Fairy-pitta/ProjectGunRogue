// GRGoodsTextWidget.cpp


#include "GRGoodsTextWidget.h"
#include "Components/TextBlock.h"

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
