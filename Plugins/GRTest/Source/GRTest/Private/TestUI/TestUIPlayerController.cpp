// Fill out your copyright notice in the Description page of Project Settings.


#include "TestUI/TestUIPlayerController.h"
#include "UI/BattleHUD/GRBattleHUDWidget.h"
#include "UI/BattleHUD/SubWidgets/GRLevelStatusWidget.h"
#include "UI/BattleHUD/SubWidgets/GRNotifyMessageWidget.h"
#include "UI/BattleHUD/SubWidgets/GRGoodsTextWidget.h"
#include "EnhancedInputSubsystems.h"
#include "UI/BattleHUD/SubWidgets/GRPlayerStatusWidget.h"
#include "UI/BattleHUD/SubWidgets/GRSkillListWidget.h"
#include "UI/BattleHUD/SubWidgets/GRSkillSlotWidget.h"
#include "UI/BattleHUD/SubWidgets/GRTeamStatusListWidget.h"
#include "UI/BattleHUD/SubWidgets/GRWeaponListWidget.h"


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

			if (UGRLevelStatusWidget* LevelStatusWidget = BattleHUDWidget->GetLevelStatusWidget())
			{
				LevelStatusWidget->SetCurrentLocText(FText::FromString(TEXT("구역 1-1")));
				LevelStatusWidget->SetDifficultyText(FText::FromString(TEXT("어려움")));
				LevelStatusWidget->SetPlayingTime(PlayingTimeSeconds);
			}

			if (UGRNotifyMessageWidget* NotifyMessageWidget = BattleHUDWidget->GetNotifyMessageWidget())
			{
				NotifyMessageWidget->SetNotifyMessage(FText::FromString(TEXT("Notify Message Part")));
			}

			if (UGRGoodsTextWidget* GoodsTextWidget = BattleHUDWidget->GetGoodsTextWidget())
			{
				GoodsTextWidget->SetGoodsNameText(FText::FromString(TEXT("Gold: ")));
				GoodsTextWidget->SetGoodsCountText(9900);
			}

			if (UGRTeamStatusListWidget* TeamStatusListWidget = BattleHUDWidget->GetTeamStatusListWidget())
			{
				TeamStatusListWidget->SetTeamShieldBar(0,50.0f, 100.0f);
				TeamStatusListWidget->SetTeamHPBar(0, 70.0f, 100.0f);
			}

			if (UGRPlayerStatusWidget* PlayerStatusWidget = BattleHUDWidget->GetPlayerStatusWidget())
			{
				PlayerStatusWidget->SetPlayerShieldBar(30.0f, 100.0f);
				PlayerStatusWidget->SetPlayerHPBar(70.0f, 100.0f);
			}

			if (UGRSkillListWidget* SkillListWidget = BattleHUDWidget->GetSkillListWidget())
			{
				if (UGRSkillSlotWidget* FirstSKillSlotWidget = SkillListWidget->GetFirstSkillSlot())
				{
					FirstSKillSlotWidget->SetSkillKey(FText::FromString("Q"));
					FirstSKillSlotWidget->SetSkillCountText(3);
				}
				if (UGRSkillSlotWidget* SecondSKillSlotWidget = SkillListWidget->GetSecondSkillSlot())
				{
					SecondSKillSlotWidget->SetSkillKey(FText::FromString("E"));
					SecondSKillSlotWidget->SetSkillCountText(5);
				}
				if (UGRSkillSlotWidget* ThirdSkillSlotWidget = SkillListWidget->GetThirdSkillSlot())
				{
					ThirdSkillSlotWidget->SetSkillKey(FText::FromString("Shift"));
					ThirdSkillSlotWidget->SetSkillCountText(0);
				}
			}
		}
	}

	GetWorldTimerManager().SetTimer(
		PlayingTimeHandle,
		this,
		&ATestUIPlayerController::UpdatePlayingTime,
		1.0f,
		true
	);
}

void ATestUIPlayerController::HandleSkillInput(FName SkillKey)
{
	if (!BattleHUDWidget)
	{
		return;
	}

	if (UGRSkillListWidget* SkillListWidget = BattleHUDWidget->GetSkillListWidget())
	{
		UGRSkillSlotWidget* TargetSlot = nullptr;

		if (SkillKey == "Q")
		{
			TargetSlot = SkillListWidget->GetFirstSkillSlot();
		}
		else if (SkillKey == "E")
		{
			TargetSlot = SkillListWidget->GetSecondSkillSlot();
		}
		else if (SkillKey == "Shift")
		{
			TargetSlot = SkillListWidget->GetThirdSkillSlot();
		}

		if (TargetSlot)
		{
			int32 CurrentCount = TargetSlot->GetSkillCount();
			if (CurrentCount > 0)
			{
				int32 NewCount = FMath::Clamp(CurrentCount - 1, 0, CurrentCount);
				TargetSlot->SetSkillCountText(NewCount);
			}

			TargetSlot->StartCooldown(5.0f);
		}
	}
}

void ATestUIPlayerController::SelectWeaponSlot(int32 Index)
{
	if (!BattleHUDWidget)
	{
		return;
	}
	
	if (UGRWeaponListWidget* WeaponListWidget = BattleHUDWidget->GetWeaponListWidget())
	{
		WeaponListWidget->SetSelectedWeapon(Index);
		WeaponListWidget->UpdateBulletCount(Index, 8, 8);
	}
}

void ATestUIPlayerController::CreateBuffIconInWrapBox()
{
	if (!BattleHUDWidget)
	{
		return;
	}

	if (UGRPlayerStatusWidget* PlayerStatusWidget = BattleHUDWidget->GetPlayerStatusWidget())
	{
		PlayerStatusWidget->CreateBuffIcon();
	}
}

void ATestUIPlayerController::CreateTeamStatusInVerticalBox()
{
	if (!BattleHUDWidget)
	{
		return;
	}

	if (UGRTeamStatusListWidget* TeamStatusListWidget = BattleHUDWidget->GetTeamStatusListWidget())
	{
		TeamStatusListWidget->CreateTeamStatus();
	}
}

void ATestUIPlayerController::UpdatePlayingTime()
{
	PlayingTimeSeconds++;

	if (BattleHUDWidget)
	{
		if (UGRLevelStatusWidget* LevelStatusWidget = BattleHUDWidget->GetLevelStatusWidget())
		{
			LevelStatusWidget->SetPlayingTime(PlayingTimeSeconds);
		}
	}
}
