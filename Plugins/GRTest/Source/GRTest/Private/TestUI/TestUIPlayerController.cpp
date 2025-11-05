// Fill out your copyright notice in the Description page of Project Settings.


#include "TestUI/TestUIPlayerController.h"
#include "UI/GRBattleHUDWidget.h"
#include "EnhancedInputSubsystems.h"

void ATestUIPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	if (BattleHUDClass)
	{
		BattleHUDWidget = CreateWidget<UGRBattleHUDWidget>(this, BattleHUDClass);
		if (BattleHUDWidget)
		{
			BattleHUDWidget->AddToViewport();
		}
	}
}
