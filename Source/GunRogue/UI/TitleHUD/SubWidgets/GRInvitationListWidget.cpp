// GRInvitationListWidget.cpp


#include "UI/TitleHUD/SubWidgets/GRInvitationListWidget.h"
#include "GRInvitationSlotWidget.h"
#include "Components/PanelSlot.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"

void UGRInvitationListWidget::CreateInvitationSlot()
{
	if (!InvitationSlotClass || !InvitationContainer)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	UGRInvitationSlotWidget* NewInvitationSlot = CreateWidget<UGRInvitationSlotWidget>(PC, InvitationSlotClass);
	if (!NewInvitationSlot)
	{
		return;
	}

	if (UPanelSlot* PanelSlot = InvitationContainer->AddChild(NewInvitationSlot))
	{
		if (UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(PanelSlot))
		{
			ScrollBoxSlot->SetPadding(FMargin(5.0f));
		}
	}

	InvitationSlots.Add(NewInvitationSlot);
}
