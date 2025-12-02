// Fill out your copyright notice in the Description page of Project Settings.


#include "TestAugment/TestAugPlayerController.h"

#include "UI/BattleHUD/GRBattleHUDWidget.h"
#include "UI/Augment/GRAugmentHUDWidget.h"

void ATestAugPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (BattleHUDClass)
	{
		BattleHUDWidget = CreateWidget<UGRBattleHUDWidget>(this, BattleHUDClass);
		if (BattleHUDWidget)
		{
			BattleHUDWidget->AddToViewport();
		}
	}
}

void ATestAugPlayerController::ShowAugmentHUD()
{
	if (AugmentHUDClass)
	{
		AugmentHUDWidget = CreateWidget<UGRAugmentHUDWidget>(this, AugmentHUDClass);
		if (AugmentHUDWidget)
		{
			AugmentHUDWidget->AddToViewport();
		}

		FInputModeUIOnly UIMode;
		SetInputMode(UIMode);
		bShowMouseCursor = true;
	}
}
