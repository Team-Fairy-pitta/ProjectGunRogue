// GRNotifyMessageWidget.cpp


#include "UI/GRNotifyMessageWidget.h"
#include "UI/GRBaseTextWidget.h"

void UGRNotifyMessageWidget::SetNotifyMessage(const FText& InText, const FLinearColor& InColor)
{
	if (!NotifyMessageText) return;

	NotifyMessageText->SetTextAndColor(
		InText,
		FSlateColor(InColor)
	);
}
