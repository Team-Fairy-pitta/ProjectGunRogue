#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"
#include "GameModes/Level1/GRGameState_Level1.h"
#include "UI/Level1/GRLevel1SelectWidget.h"

void AGRBattlePlayerController::ClientRPC_ShowLevel1SelectWidget_Implementation(AGRLevel1ControlPanel* ControlPanel)
{
	if (!GetWorld())
	{
		return;
	}

	AGRGameState_Level1* GRGameState = GetWorld()->GetGameState<AGRGameState_Level1>();
	if (!IsValid(GRGameState))
	{
		UE_LOG(LogTemp, Error, TEXT("AGRGameState_Level1 is INVALID"));
		return;
	}

	CachedControlPanel = ControlPanel;
	OnReceiveHandle = GRGameState->OnReceiveNextRoomInformation.AddUObject(this, &ThisClass::OnReceiveNextRoomInformation);
	ServerRPC_RequestNextRoomInformation();
}

void AGRBattlePlayerController::ServerRPC_RequestNextRoomInformation_Implementation()
{
	if (!GetWorld())
	{
		return;
	}

	AGRGameState_Level1* GRGameState = GetWorld()->GetGameState<AGRGameState_Level1>();
	if (!IsValid(GRGameState))
	{
		UE_LOG(LogTemp, Error, TEXT("AGRGameState_Level1 is INVALID"));
		return;
	}

	GRGameState->RequestNextRoomInformation();
}

void AGRBattlePlayerController::OnReceiveNextRoomInformation()
{
	AGRGameState_Level1* GRGameState = GetWorld()->GetGameState<AGRGameState_Level1>();
	GRGameState->OnReceiveNextRoomInformation.Remove(OnReceiveHandle);

	ShowLevel1SelectWidget();
	SetLevel1SelectWidget(GRGameState->GetLevel1ClientData(), CachedControlPanel);

	CachedControlPanel = nullptr;
	OnReceiveHandle.Reset();
}

void AGRBattlePlayerController::SetLevel1SelectWidget(const FGRLevel1Data& Level1Data, AGRLevel1ControlPanel* ControlPanel)
{
	if (!Level1SelectWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Level1SelectWidgetInstance is INVALID"));
		return;
	}

	Level1SelectWidgetInstance->InitWidget(Level1Data, ControlPanel);
}

void AGRBattlePlayerController::ShowLevel1SelectWidget()
{
	if (!Level1SelectWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Level1SelectWidgetInstance is INVALID"));
		return;
	}
	if (!Level1SelectWidgetInstance->IsInViewport())
	{
		Level1SelectWidgetInstance->AddToViewport();
	}

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(Level1SelectWidgetInstance->GetCachedWidget());
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

void AGRBattlePlayerController::HideLevel1SelectWidget()
{
	if (!Level1SelectWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Level1SelectWidgetInstance is INVALID"));
		return;
	}
	if (Level1SelectWidgetInstance->IsInViewport())
	{
		Level1SelectWidgetInstance->RemoveFromParent();
		Level1SelectWidgetInstance->ResetWidget();
	}

	CachedControlPanel = nullptr;
	OnReceiveHandle.Reset();

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}
