#include "UI/Level1/GRLevel1SelectWidget.h"
#include "Player/Battle/GRBattlePlayerController.h"

void UGRLevel1SelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetWidgetFocusable();
}

FReply UGRLevel1SelectWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		APlayerController* PlayerController = GetOwningPlayer();
		AGRBattlePlayerController* BattlePlayerController = Cast<AGRBattlePlayerController>(PlayerController);

		if (IsValid(BattlePlayerController))
		{
			BattlePlayerController->HideLevel1SelectWidget();
		}

		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UGRLevel1SelectWidget::SetWidgetFocusable()
{
	bIsFocusable = true;
}
