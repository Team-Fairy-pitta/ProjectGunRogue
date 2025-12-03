#include "GRWeaponUpgradeWidgetSetting.h"

#include "Player/Battle/GRBattlePlayerController.h"

void UGRWeaponUpgradeWidgetSetting::NativeConstruct()
{
	Super::NativeConstruct();

	SetWidgetFocusable();
}

void UGRWeaponUpgradeWidgetSetting::NativeDestruct()
{
	Super::NativeDestruct();
}

FReply UGRWeaponUpgradeWidgetSetting::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			if (AGRBattlePlayerController* BattlePC = Cast<AGRBattlePlayerController>(PC))
			{
				BattlePC->HideUpgradeConsoleWidget();
				return FReply::Handled();
			}
		}
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UGRWeaponUpgradeWidgetSetting::SetWidgetFocusable()
{
	SetIsFocusable(true);
}
