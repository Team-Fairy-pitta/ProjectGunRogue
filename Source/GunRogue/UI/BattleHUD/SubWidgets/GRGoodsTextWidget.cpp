// GRGoodsTextWidget.cpp


#include "GRGoodsTextWidget.h"
#include "Components/TextBlock.h"

void UGRGoodsTextWidget::SetGoodsNameText(const FText& InText)
{
	if (!GoodsNameText)
	{
		return;
	}

	GoodsNameText->SetText(InText);
}

void UGRGoodsTextWidget::SetGoodsCountText(int32 InCount)
{
	if (!GoodsCountText)
	{
		return;
	}

	FString CountString = FString::Printf(TEXT("%d"), InCount);
	
	GoodsCountText->SetText(FText::FromString(CountString));
}
