// GRGoodsTextWidget.cpp


#include "UI/GRGoodsTextWidget.h"
#include "UI/GRBaseTextWidget.h"

void UGRGoodsTextWidget::SetGoodsNameText(const FText& InText, const FLinearColor& InColor)
{
	if (!GoodsNameText) return;

	GoodsNameText->SetTextAndColor(
		InText,
		FSlateColor(InColor)
	);
}

void UGRGoodsTextWidget::SetGoodsCountText(int32 InCount, const FLinearColor& InColor)
{
	if (!GoodsCountText) return;

	FString CountString = FString::Printf(TEXT("%d"), InCount);
	
	GoodsCountText->SetTextAndColor(
		FText::FromString(CountString),
		FSlateColor(InColor)
	);
}
