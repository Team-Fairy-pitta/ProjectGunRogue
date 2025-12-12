// GRLobbyPlayerSlotWidget.cpp


#include "UI/TitleHUD/SubWidgets/GRLobbyPlayerSlotWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UGRLobbyPlayerSlotWidget::SetPlayerNameText(const FText& InText)
{
	if (!PlayerNameText)
	{
		return;
	}

	PlayerNameText->SetText(InText);
}

void UGRLobbyPlayerSlotWidget::SetPlayerIcon(UTexture2D* InTexture)
{
	if (!PlayerIcon)
	{
		return;
	}

	PlayerIcon->SetBrushFromTexture(InTexture);
}
