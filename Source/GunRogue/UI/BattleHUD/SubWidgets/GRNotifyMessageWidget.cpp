// GRNotifyMessageWidget.cpp


#include "GRNotifyMessageWidget.h"
#include "UI/Common/GRBaseTextWidget.h"

void UGRNotifyMessageWidget::SetNotifyMessage(const FText& InText)
{
	if (!NotifyMessageText) return;

	NotifyMessageText->SetText(InText);
}
