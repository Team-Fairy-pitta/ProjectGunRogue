// GRLobbyPlayerSlotWidget.cpp


#include "UI/TitleHUD/SubWidgets/GRLobbyPlayerSlotWidget.h"
#include "Components/TextBlock.h"

void UGRLobbyPlayerSlotWidget::SetPlayerNameText(const FText& InText)
{
	if (!PlayerNameText)
	{
		return;
	}

	PlayerNameText->SetText(InText);
}
