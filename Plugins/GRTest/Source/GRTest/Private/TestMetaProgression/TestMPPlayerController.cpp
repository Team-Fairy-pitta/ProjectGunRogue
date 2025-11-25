// Fill out your copyright notice in the Description page of Project Settings.


#include "TestMetaProgression/TestMPPlayerController.h"

#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "MetaProgression/GRPerkSubsystem.h"
#include "TestMetaProgression/TestMPPlayerState.h"
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

void ATestMPPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (HasAuthority())
	{
		UGRPerkSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UGRPerkSubsystem>();
		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPlayerState<ATestMPPlayerState>());

		if (Subsystem && ASC)
		{
			Subsystem->ApplyAllPerksToASC(ASC, PerkTable, PerkGE);
		}
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

