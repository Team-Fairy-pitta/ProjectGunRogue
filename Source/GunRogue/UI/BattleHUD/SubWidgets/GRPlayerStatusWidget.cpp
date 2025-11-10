// GRPlayerStatusWidget.cpp


#include "GRPlayerStatusWidget.h"

#include "GRBuffIconWidget.h"
#include "GRHPBarWidget.h"
#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"

void UGRPlayerStatusWidget::SetPlayerShieldBar(float CurrentShield, float MaxShield)
{
	if (!PlayerHPBar)
	{
		return;
	}

	PlayerHPBar->SetShieldBar(CurrentShield, MaxShield);
}

void UGRPlayerStatusWidget::SetPlayerHPBar(float CurrentHP, float MaxHP)
{
	if (!PlayerHPBar)
	{
		return;
	}

	PlayerHPBar->SetHPBar(CurrentHP, MaxHP);
}

void UGRPlayerStatusWidget::CreateBuffIcon()
{
	if (!BuffIconClass || !BuffIconContainer)
	{
		return;
	}
	
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}
	
	UGRBuffIconWidget* NewBuffIcon = CreateWidget<UGRBuffIconWidget>(PC, BuffIconClass);
	if (!NewBuffIcon)
	{
		return;
	}

	if (UWrapBoxSlot* WrapBoxSlot = BuffIconContainer->AddChildToWrapBox(NewBuffIcon))
	{
		WrapBoxSlot->SetPadding(FMargin(2.0f));
		WrapBoxSlot->SetHorizontalAlignment(HAlign_Left);
		WrapBoxSlot->SetVerticalAlignment(VAlign_Center);
	}
	
	BuffIcons.Add(NewBuffIcon);
}
