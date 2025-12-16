// GRLobbyBaseButtonWidget.cpp


#include "UI/TitleHUD/SubWidgets/GRLobbyBaseButtonWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "GRLobbyCharacterSelectSlotWidget.h"

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

void UGRLobbyBaseButtonWidget::SetButtonText(FText InText)
{
	if (BaseButtonText)
	{
		BaseButtonText->SetText(InText);
	}
}

void UGRLobbyBaseButtonWidget::EnableButton()
{
	if (BaseButton)
	{
		BaseButton->SetIsEnabled(true);
	}
}

void UGRLobbyBaseButtonWidget::DisableButton()
{
	if (BaseButton)
	{
		BaseButton->SetIsEnabled(false);
	}
}

void UGRLobbyBaseButtonWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (BaseButton)
	{
		BaseButton->OnClicked.RemoveDynamic(this, &UGRLobbyBaseButtonWidget::OnBaseClicked);
	}
}

void UGRLobbyBaseButtonWidget::OnBaseClicked()
{
	OnLobbyButtonClicked.Broadcast();
}
