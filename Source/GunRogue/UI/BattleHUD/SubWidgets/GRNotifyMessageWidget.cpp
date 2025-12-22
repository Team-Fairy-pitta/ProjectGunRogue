// GRNotifyMessageWidget.cpp


#include "GRNotifyMessageWidget.h"
#include "Components/TextBlock.h"

void UGRNotifyMessageWidget::OnAnimFadeInFinished()
{
	if (Fade)
	{
		FWidgetAnimationDynamicEvent EndDelegateFade;
		EndDelegateFade.BindDynamic(this, &UGRNotifyMessageWidget::OnAnimFadeFinished);
		BindToAnimationFinished(Fade, EndDelegateFade);

		float AnimationSpeed = 1 / MessageTime;
		PlayAnimation(Fade,0.0f,1,EUMGSequencePlayMode::Forward,AnimationSpeed);
	}
}

void UGRNotifyMessageWidget::OnAnimFadeFinished()
{
	if (FadeOut)
	{
		PlayAnimation(FadeOut);
	}
}

void UGRNotifyMessageWidget::SetNotifyMessage(const FText& InText, float ShowMessageTime)
{
	if (!NotifyMessageText)
	{
		return;
	}

	if (!FadeIn)
	{
		return;
	}
	if (ShowMessageTime)
	{
		MessageTime = ShowMessageTime;
	}

	NotifyMessageText->SetText(InText);

	UnbindAllFromAnimationFinished(FadeIn);
	UnbindAllFromAnimationFinished(Fade);
	if (FadeIn)
	{
		FWidgetAnimationDynamicEvent EndDelegateFadeIn;
		EndDelegateFadeIn.BindDynamic(this, &UGRNotifyMessageWidget::OnAnimFadeInFinished);
		BindToAnimationFinished(FadeIn, EndDelegateFadeIn);
		PlayAnimation(FadeIn);
	}
	
}
