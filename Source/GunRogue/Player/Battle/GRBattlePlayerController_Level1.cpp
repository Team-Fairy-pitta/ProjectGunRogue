#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"
#include "GameModes/Level1/GRGameState_Level1.h"
#include "GameModes/Level1/GRGameMode_Level1.h"
#include "System/GRLevel1ControlPanel.h"
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

	ShowLevel1SelectWidget();
	SetLevel1SelectWidget(GRGameState->GetLevel1ClientData(), ControlPanel);
}

void AGRBattlePlayerController::ClientRPC_HideLevel1SelectWidget_Implementation()
{
	HideLevel1SelectWidget();
}

void AGRBattlePlayerController::ServerRPC_OnSelectNextRoom_Implementation(int32 NextRoomIndex, AGRLevel1ControlPanel* ControlPanel)
{
	if (!HasAuthority())
	{
		return;
	}
	if (!GetWorld())
	{
		return;
	}

	if (!IsValid(ControlPanel))
	{
		UE_LOG(LogTemp, Error, TEXT("ControlPanel is INVALID"));
		return;
	}
	
	AGameStateBase* GameState = GetWorld()->GetGameState();
	AGRGameState_Level1* GameState_Level1 = Cast<AGRGameState_Level1>(GameState);
	if (!IsValid(GameState_Level1))
	{
		return;
	}

	GameState_Level1->SetCurrentRoomIndex(NextRoomIndex);
	GameState_Level1->RequestNextRoomInformation();
	FGRLevel1Node* NextNode = GameState_Level1->GetCurrentNodeInfo();

	ControlPanel->OnUsePanel(*NextNode);

	for (APlayerState* PS: GameState->PlayerArray)
	{
		if (IsValid(PS))
		{
			AGRBattlePlayerController* BattleController = Cast<AGRBattlePlayerController>(PS->GetOwner());
			if (IsValid(BattleController))
			{
				BattleController->ClientRPC_HideLevel1SelectWidget();
			}
		}
	}

	AGRGameMode_Level1* GameMode_Level1 = GetWorld()->GetAuthGameMode<AGRGameMode_Level1>();
	if (!IsValid(GameMode_Level1))
	{
		return;
	}

	GameMode_Level1->BroadcastOpenNextStage();
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

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}
