// GRNotifyMessageWidget.cpp


#include "GRNotifyMessageWidget.h"
#include "Components/TextBlock.h"

void UGRNotifyMessageWidget::SetNotifyMessage(const FText& InText)
{
	if (!NotifyMessageText)
	{
		return;
	}

	if (!Fade)
	{
		return;
	}

	NotifyMessageText->SetText(InText);
	PlayAnimation(Fade);
}
