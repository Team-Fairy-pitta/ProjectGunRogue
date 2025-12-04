#include "Player/GameStart/GRGameStart_PlayerController.h"
#include "UI/TitleHUD/GRTitleHUDWidget.h"
#include "UI/TitleHUD/GRSettingWidget.h"

AGRGameStart_PlayerController::AGRGameStart_PlayerController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGRGameStart_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		CreateWidgets();
		ShowTitleWidget();
	}
}

void AGRGameStart_PlayerController::CreateWidgets()
{
	if (!TitleWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("TitleWidgetClass (TSubclassOf<UGRTitleHUDWidget>) is INVALID"));
		return;
	}

	TitleWidgetInstance = CreateWidget<UGRTitleHUDWidget>(this, TitleWidgetClass);
	if (!TitleWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("CANNOT Create UGRTitleHUDWidget Widgets"));
		return;
	}

	if (!SettingWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SettingWidgetClass (TSubclassOf<UGRSettingWidget>) is INVALID"));
		return;
	}

	SettingWidgetInstance = CreateWidget<UGRSettingWidget>(this, SettingWidgetClass);
	if (!SettingWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("CANNOT Create UGRTitleHUDWidget Widgets"));
		return;
	}
}

void AGRGameStart_PlayerController::ShowTitleWidget()
{
	if (!TitleWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("TitleWidgetInstance is INVALID"));
		return;
	}
	if (!TitleWidgetInstance->IsInViewport())
	{
		TitleWidgetInstance->AddToViewport();
	}

	FInputModeUIOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

void AGRGameStart_PlayerController::ShowSettingWidget()
{
	if (!SettingWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("SettingWidgetInstance is INVALID"));
		return;
	}
	if (!SettingWidgetInstance->IsInViewport())
	{
		SettingWidgetInstance->AddToViewport();
	}
}

void AGRGameStart_PlayerController::HideSettingWidget()
{
	if (!SettingWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("SettingWidgetInstance is INVALID"));
		return;
	}
	if (SettingWidgetInstance->IsInViewport())
	{
		SettingWidgetInstance->RemoveFromParent();
	}
}
