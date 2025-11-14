// GRLobbyPlayerListWidget.cpp


#include "UI/TitleHUD/SubWidgets/GRLobbyPlayerListWidget.h"
#include "GRLobbyPlayerSlotWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UGRLobbyPlayerListWidget::CreateLobbyPlayerSlot()
{
	if (!LobbyPlayerSlotClass || !LobbyPlayerContainer)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	UGRLobbyPlayerSlotWidget* NewLobbyPlayerSlot = CreateWidget<UGRLobbyPlayerSlotWidget>(PC, LobbyPlayerSlotClass);
	if (!NewLobbyPlayerSlot)
	{
		return;
	}

	if (UVerticalBoxSlot* VerticalSlot = LobbyPlayerContainer->AddChildToVerticalBox(NewLobbyPlayerSlot))
	{
		VerticalSlot->SetPadding(FMargin(0,5));
		VerticalSlot->SetVerticalAlignment(VAlign_Top);
	}

	LobbyPlayerSlots.Add(NewLobbyPlayerSlot);
}
