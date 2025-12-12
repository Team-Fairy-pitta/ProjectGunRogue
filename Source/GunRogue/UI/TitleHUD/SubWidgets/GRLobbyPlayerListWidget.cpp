// GRLobbyPlayerListWidget.cpp


#include "UI/TitleHUD/SubWidgets/GRLobbyPlayerListWidget.h"
#include "GRLobbyPlayerSlotWidget.h"
#include "Player/Lobby/GRLobbyPlayerState.h"
#include "Character/GRCharacter.h"
#include "Character/GRPawnData.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UGRLobbyPlayerListWidget::UpdateHostPlayerInfo(FHostPlayer& HostPlayer)
{
	if (!HostPlayer.PlayerState)
	{
		return;
	}

	if (LobbyPlayerSlots.Num() < 1)
	{
		CreateLobbyPlayerSlot();
	}

	FString PlayerName = HostPlayer.PlayerState->GetPlayerName();
	FText PlayerNameText = FText::FromString(FString::Printf(TEXT("[Host] %s"), *PlayerName));
	
	LobbyPlayerSlots[0]->SetPlayerNameText(PlayerNameText);

	AGRCharacter* CDO = HostPlayer.SelectedCharacterClass.GetDefaultObject();
	if (CDO && CDO->PawnData)
	{
		LobbyPlayerSlots[0]->SetPlayerIcon(CDO->PawnData->CharacterThumbnail);
	}
	else
	{
		LobbyPlayerSlots[0]->SetPlayerIcon(nullptr);
	}
}

void UGRLobbyPlayerListWidget::UpdateGuestPlayersInfo(TArray<FGuestPlayer>& GuestPlayers)
{
	// Host + Guest
	ResizeLobbyPlayerSlot(GuestPlayers.Num() + 1);

	int32 Index = 1;
	for (const FGuestPlayer& Guest : GuestPlayers)
	{
		if (!Guest.PlayerState)
		{
			continue;
		}

		FString PlayerName = Guest.PlayerState->GetPlayerName();
		FString IsReadyText = Guest.bIsReady ? TEXT("Ready!") : TEXT("...");
		FText PlayerNameText = FText::FromString(FString::Printf(TEXT("%s %s"), *PlayerName, *IsReadyText));
		
		LobbyPlayerSlots[Index]->SetPlayerNameText(PlayerNameText);

		AGRCharacter* CDO = Guest.SelectedCharacterClass.GetDefaultObject();
		if (CDO && CDO->PawnData)
		{
			LobbyPlayerSlots[Index]->SetPlayerIcon(CDO->PawnData->CharacterThumbnail);
		}
		else
		{
			LobbyPlayerSlots[Index]->SetPlayerIcon(nullptr);
		}

		Index += 1;
	}
}

void UGRLobbyPlayerListWidget::CreateLobbyPlayerSlot()
{
	if (!LobbyPlayerSlotClass || !LobbyPlayerContainer)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	UGRLobbyPlayerSlotWidget* NewLobbyPlayerSlot = CreateWidget<UGRLobbyPlayerSlotWidget>(PC, LobbyPlayerSlotClass);
	if (!NewLobbyPlayerSlot)
	{
		return;
	}

	if (UVerticalBoxSlot* VerticalSlot = LobbyPlayerContainer->AddChildToVerticalBox(NewLobbyPlayerSlot))
	{
		VerticalSlot->SetPadding(FMargin(0,5));
		VerticalSlot->SetVerticalAlignment(VAlign_Top);
	}

	LobbyPlayerSlots.Add(NewLobbyPlayerSlot);
}

void UGRLobbyPlayerListWidget::ClearLobbyPlayerSlot()
{
	LobbyPlayerContainer->ClearChildren();

	for (UGRLobbyPlayerSlotWidget* PlayerSlot : LobbyPlayerSlots)
	{
		PlayerSlot->RemoveFromParent();
	}
	LobbyPlayerSlots.Empty();
}

void UGRLobbyPlayerListWidget::ResizeLobbyPlayerSlot(int32 Num)
{
	if (!LobbyPlayerSlotClass || !LobbyPlayerContainer)
	{
		return;
	}

	APlayerController* PlayerController = GetOwningPlayer();
	if (!PlayerController)
	{
		return;
	}

	while (LobbyPlayerSlots.Num() > Num)
	{
		int32 LastIndex = LobbyPlayerSlots.Num() - 1;
		LobbyPlayerSlots.RemoveAt(LastIndex);
		LobbyPlayerContainer->RemoveChildAt(LastIndex);
	}

	while (LobbyPlayerSlots.Num() < Num)
	{
		CreateLobbyPlayerSlot();
	}
}
