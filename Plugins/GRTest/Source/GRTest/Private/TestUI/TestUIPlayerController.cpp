// Fill out your copyright notice in the Description page of Project Settings.


#include "TestUI/TestUIPlayerController.h"
#include "UI/GRBattleHUDWidget.h"

void ATestUIPlayerController::BeginPlay()
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
