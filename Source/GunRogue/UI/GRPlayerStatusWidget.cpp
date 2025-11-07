// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GRPlayerStatusWidget.h"

#include "GRBuffIconWidget.h"
#include "GRHPBarWidget.h"
#include "Components/HorizontalBox.h"

void UGRPlayerStatusWidget::SetPlayerShieldBar(float CurrentShield, float MaxShield)
{
	if (!PlayerHPBar) return;

	PlayerHPBar->SetShieldBar(CurrentShield, MaxShield);
}

void UGRPlayerStatusWidget::SetPlayerHPBar(float CurrentHP, float MaxHP)
{
	if (!PlayerHPBar) return;

	PlayerHPBar->SetHPBar(CurrentHP, MaxHP);
}

void UGRPlayerStatusWidget::CreateBuffIcon()
{
	if (!BuffIconClass) return;

	
	UWorld* World = GetWorld();
	if (!World) return;
	
	UGRBuffIconWidget* NewBuffIcon = CreateWidget<UGRBuffIconWidget>(World, BuffIconClass);
	if (!NewBuffIcon) return;
	
	if (BuffIconContainer)
	{
		BuffIconContainer->AddChild(NewBuffIcon);
	}
	
	BuffIcons.Add(NewBuffIcon);
}
