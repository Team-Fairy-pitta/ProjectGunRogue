#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/Battle/GRBattleCheatManager.h"
#include "Player/GRPlayerState.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "UI/BattleHUD/GRBattleHUDWidget.h"
#include "UI/Level1/GRLevel1SelectWidget.h"
#include "UI/Weapon/GRWeaponUpgradeWidgetSetting.h"
#include "UI/Inventory/GRInventoryWidgetMain.h"

AGRBattlePlayerController::AGRBattlePlayerController()
{
#if WITH_EDITOR
	CheatClass = UGRBattleCheatManager::StaticClass();
#endif
}

void AGRBattlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Standalone 모드에서 테스트하는 경우, OnRep_PlayerState()을 직접 호출해 주어야 한다.
	// 또한, Editor가 아닌 곳에서 Standalone으로 실행되어서는 안된다.
	if (GetNetMode() == ENetMode::NM_Standalone)
	{
#if WITH_EDITOR
		OnRep_PlayerState();
#else
		UE_LOG(LogTemp, Fatal, TEXT("BattlePlayerController: requires ListenServer or Client!!"));
#endif
	}
	// ListenServer 모드로 실행된 서버의 경우에도, OnRep_PlayerState()을 직접 호출해 주어야 한다.
	// 서버에서는 PlayerController가 먼저 Spawn되고, 그 이후 PlayerState가 Spawn되므로 문제가 없다.
	else if (GetNetMode() == ENetMode::NM_ListenServer)
	{
		if (IsLocalController())
		{
			OnRep_PlayerState();
		}
	}
}

void AGRBattlePlayerController::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	FinalizeBattleHUD();
}

void AGRBattlePlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// 클라이언트에 PlayerController와 PlayerState가 모두 준비되어야 HUD를 초기화할 수 있다.

	AGRPlayerState* GRPlayerState = GetPlayerState<AGRPlayerState>();
	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerState (AGRPlayerState) is INVALID"));
		return;
	
	}
	
	if (GRPlayerState->IsAbilitySystemComponentInit())
	{
		InitUISetup();
	}
	else
	{
		GRPlayerState->OnAbilitySystemComponentInit.AddUObject(this, &ThisClass::InitUISetup);
	}
}

void AGRBattlePlayerController::InitUISetup()
{
	CreateWidgets();
	InitializeBattleHUD();
	ShowBattleHUD();
}

void AGRBattlePlayerController::CreateWidgets()
{
	if(!HUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetClass (TSubclassOf<>) is INVALID"));
		return;
	}

	HUDWidgetInstance = CreateWidget<UGRBattleHUDWidget>(this, HUDWidgetClass);
	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("CANNOT Create UGRBattleHUDWidget Widgets"));
		return;
	}

	if (!Level1SelectWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Level1SelectWidgetClass (TSubclassOf<>) is INVALID"));
		return;
	}

	Level1SelectWidgetInstance = CreateWidget<UGRLevel1SelectWidget>(this, Level1SelectWidgetClass);
	if (!Level1SelectWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("CANNOT Create Level1SelectWidget Widgets"));
		return;
	}

	if (!UpgradeConsoleWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UpgradeConsoleWidgetClass (TSubclassOf<UGRWeaponUpgradeWidgetSetting>) is INVALID"));
		return;
	}

	UpgradeConsoleWidgetInstance = CreateWidget<UGRWeaponUpgradeWidgetSetting>(this, UpgradeConsoleWidgetClass);
	if (!UpgradeConsoleWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("CANNOT Create UGRWeaponUpgradeWidgetSetting Widgets"));
		return;
	}

	if (!InventoryWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidgetClass (TSubclassOf<UGRInventoryWidgetMain>) is INVALID"));
		return;
	}

	InventoryWidgetInstance = CreateWidget<UGRInventoryWidgetMain>(this, InventoryWidgetClass);
	if (!InventoryWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("CANNOT Create UGRInventoryWidgetMain Widgets"));
		return;
	}
}
