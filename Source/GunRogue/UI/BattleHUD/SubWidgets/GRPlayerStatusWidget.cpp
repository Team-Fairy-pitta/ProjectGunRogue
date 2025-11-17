// GRPlayerStatusWidget.cpp


#include "GRPlayerStatusWidget.h"

#include "GRBuffIconWidget.h"
#include "GRHPBarWidget.h"
#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"

void UGRPlayerStatusWidget::SetPlayerShieldBar(float CurrentShield, float MaxShield)
{
	SetPlayerShield(CurrentShield);
	SetPlayerMaxShield(MaxShield);
}

void UGRPlayerStatusWidget::SetPlayerHPBar(float CurrentHP, float MaxHP)
{
	SetPlayerHealth(CurrentHP);
	SetPlayerMaxHealth(MaxHP);
}

void UGRPlayerStatusWidget::SetPlayerHealth(float Value)
{
	if (!PlayerHPBar)
	{
		return;
	}
	PlayerHPBar->SetHealth(Value);
}

void UGRPlayerStatusWidget::SetPlayerMaxHealth(float Value)
{
	if (!PlayerHPBar)
	{
		return;
	}
	PlayerHPBar->SetMaxHealth(Value);
}

void UGRPlayerStatusWidget::SetPlayerShield(float Value)
{
	if (!PlayerHPBar)
	{
		return;
	}
	PlayerHPBar->SetShield(Value);
}

void UGRPlayerStatusWidget::SetPlayerMaxShield(float Value)
{
	if (!PlayerHPBar)
	{
		return;
	}
	PlayerHPBar->SetMaxShield(Value);
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
