#include "Player/Battle/GRBattlePlayerController.h"
#include "UI/BattleHUD/GRBattleHUDWidget.h"

AGRBattlePlayerController::AGRBattlePlayerController()
{
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
	else if (GetNetMode() == ENetMode::NM_ListenServer)
	{
		if (IsLocalController())
		{
			OnRep_PlayerState();
		}
	}
}

void AGRBattlePlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// 클라이언트에 PlayerController와 PlayerState가 모두 준비되어야 HUD를 초기화할 수 있다.
	CreateWidgets();
	InitBattleHUD();

	ShowBattleHUD();
}

void AGRBattlePlayerController::ShowBattleHUD()
{
	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetInstance is INVALID"));
		return;
	}
	if (!HUDWidgetInstance->IsInViewport())
	{
		HUDWidgetInstance->AddToViewport();
	}

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}

void AGRBattlePlayerController::CreateWidgets()
{
	if(!HUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetClass (TSubclassOf<UGRBattleHUDWidget>) is INVALID"));
		return;
	}

	HUDWidgetInstance = CreateWidget<UGRBattleHUDWidget>(this, HUDWidgetClass);
	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("CANNOT Create UGRBattleHUDWidget Widgets"));
		return;
	}
}

void AGRBattlePlayerController::InitBattleHUD()
{
}
