// GRLobbyBaseButtonWidget.cpp


#include "UI/TitleHUD/SubWidgets/GRLobbyBaseButtonWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UGRLobbyBaseButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (BaseButtonText)
	{
		BaseButtonText->SetText(DefaultText);
	}

	if (BottomBar)
	{
		BottomBar->SetColorAndOpacity(DefaultColor);
	}
}

void UGRLobbyBaseButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BaseButton)
	{
		BaseButton->OnClicked.AddDynamic(this, &UGRLobbyBaseButtonWidget::OnBaseClicked);
	}
}

void UGRLobbyBaseButtonWidget::OnBaseClicked()
{
	OnLobbyButtonClicked.Broadcast();
}
