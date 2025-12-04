#include "UI/Inventory/GRInventoryWidgetMain.h"
#include "UI/Inventory/GRInventoryItemWidget.h"
#include "Player/Battle/GRBattlePlayerController.h"

void UGRInventoryWidgetMain::UpdateInventorySubWidgets()
{
	if (!InventoryItemWidget)
	{
		return;
	}

	InventoryItemWidget->UpdateInventoryDisplay();
}

void UGRInventoryWidgetMain::NativeConstruct()
{
	Super::NativeConstruct();

	SetWidgetFocusable();
}

void UGRInventoryWidgetMain::NativeDestruct()
{
	Super::NativeDestruct();
}

FReply UGRInventoryWidgetMain::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	bool bPressESC = InKeyEvent.GetKey() == EKeys::Escape;
	bool bPressTab = InKeyEvent.GetKey() == EKeys::Tab;
	if (bPressESC || bPressTab)
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			if (AGRBattlePlayerController* BattlePC = Cast<AGRBattlePlayerController>(PC))
			{
				BattlePC->HideInventoryWidget();
				return FReply::Handled();
			}
		}
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UGRInventoryWidgetMain::SetWidgetFocusable()
{
	SetIsFocusable(true);
}
