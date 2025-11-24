// Fill out your copyright notice in the Description page of Project Settings.


#include "TestMetaProgression/TestMPPlayerController.h"
#include "MetaProgression/GRPerkSubsystem.h"
#include "UI/MetaProgression/GRPerkHUDWidget.h"

void ATestMPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UGRPerkSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UGRPerkSubsystem>();
	if (Subsystem)
	{
		Subsystem->LoadPerks();
	}
	
	if (PerkHUDClass)
	{
		PerkHUDWidget = CreateWidget<UGRPerkHUDWidget>(this, PerkHUDClass);
		if (PerkHUDWidget)
		{
			PerkHUDWidget->AddToViewport();
		}

		FInputModeGameAndUI Mode;
		SetInputMode(Mode);
		bShowMouseCursor = true;
	}
	
}

void ATestMPPlayerController::SetMetaGoodsInText()
{
	UGRPerkSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UGRPerkSubsystem>();
	if (Subsystem)
	{
		Subsystem->SetMetaGoods(9000);
	}

	if (PerkHUDWidget)
	{
		PerkHUDWidget->UpdateGoodsText();
	}
}

