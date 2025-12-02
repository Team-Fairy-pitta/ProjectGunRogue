// Fill out your copyright notice in the Description page of Project Settings.


#include "TestAugment/TestAugPlayerController.h"

#include "UI/BattleHUD/GRBattleHUDWidget.h"
#include "UI/Augment/GRAugmentHUDWidget.h"

void ATestAugPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CreateWidgets();
	ShowBattleHUD();
}

void ATestAugPlayerController::ShowAugmentHUD()
{
	if (!AugmentHUDWidget)
	{
		return;
	}

	if (!AugmentHUDWidget->IsInViewport())
	{
		AugmentHUDWidget->AddToViewport();
	}

	FInputModeUIOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

void ATestAugPlayerController::ShowBattleHUD()
{
	if (!BattleHUDWidget)
	{
		return;
	}

	if (!BattleHUDWidget->IsInViewport())
	{
		BattleHUDWidget->AddToViewport();
	}

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}

void ATestAugPlayerController::CreateWidgets()
{
	if (!BattleHUDClass || !AugmentHUDClass)
	{
		return;
	}

	BattleHUDWidget= CreateWidget<UGRBattleHUDWidget>(this, BattleHUDClass);
	if (!BattleHUDWidget)
	{
		return;
	}

	AugmentHUDWidget = CreateWidget<UGRAugmentHUDWidget>(this, AugmentHUDClass);
	if (!AugmentHUDWidget)
	{
		return;
	}
}
