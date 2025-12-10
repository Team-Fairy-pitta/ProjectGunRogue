// GRLobbyHUDWidget.cpp


#include "UI/TitleHUD/GRLobbyHUDWidget.h"

#include "Components/Border.h"
#include "SubWidgets/GRLobbyBaseButtonWidget.h"
#include "UI/TitleHUD/SubWidgets/GRLobbyCharacterSelectSlotWidget.h"
#include "UI/SteamInvite/GRSteamFriendsList.h"
#include "Components/SizeBox.h"
#include "Player/Lobby/GRLobbyPlayerController.h"

void UGRLobbyHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CharacterSlots = {
		FirstCharacterSlot,
		SecondCharacterSlot,
		ThirdCharacterSlot,
		FourthCharacterSlot,
		FifthCharacterSlot,
		SixthCharacterSlot
	};

	if (PlayerInfoButton)
	{
		PlayerInfoButton->OnLobbyButtonClicked.AddDynamic(this, &ThisClass::OnPlayerInfoClicked);
	}
	if (PlayerPerksButton)
	{
		PlayerPerksButton->OnLobbyButtonClicked.AddDynamic(this, &ThisClass::OnPlayerPerksClicked);
	}
	if (StartGameButton)
	{
		StartGameButton->SetVisibility(ESlateVisibility::Visible);
		StartGameButton->OnLobbyButtonClicked.AddDynamic(this, &ThisClass::OnStartGameClicked);
	}
	if (ReadyGameButton)
	{
		ReadyGameButton->SetVisibility(ESlateVisibility::Collapsed);
		ReadyGameButton->OnLobbyButtonClicked.AddDynamic(this, &ThisClass::OnReadyGameClicked);
	}
	if (ExitLobbyButton)
	{
		ExitLobbyButton->OnLobbyButtonClicked.AddDynamic(this, &ThisClass::OnExitLobbyClicked);
	}
	if (InviteButton)
	{
		InviteButton->OnLobbyButtonClicked.AddDynamic(this, &ThisClass::OnInviteClicked);
	}
	
	for (int32 i = 0; i < CharacterSlots.Num(); ++i)
	{
		if (CharacterSlots[i])
		{
			CharacterSlots[i]->CharacterIndex = i;
			CharacterSlots[i]->OnCharacterSelectClicked.AddDynamic(this, &UGRLobbyHUDWidget::OnCharacterSelected);
		}
	}
}

void UGRLobbyHUDWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (PlayerInfoButton)
	{
		PlayerInfoButton->OnLobbyButtonClicked.RemoveDynamic(this, &ThisClass::OnPlayerInfoClicked);
	}
	if (PlayerPerksButton)
	{
		PlayerPerksButton->OnLobbyButtonClicked.RemoveDynamic(this, &ThisClass::OnPlayerPerksClicked);
	}
	if (StartGameButton)
	{
		StartGameButton->OnLobbyButtonClicked.RemoveDynamic(this, &ThisClass::OnStartGameClicked);
	}
	if (ReadyGameButton)
	{
		ReadyGameButton->OnLobbyButtonClicked.RemoveDynamic(this, &ThisClass::OnReadyGameClicked);
	}
	if (ExitLobbyButton)
	{
		ExitLobbyButton->OnLobbyButtonClicked.RemoveDynamic(this, &ThisClass::OnExitLobbyClicked);
	}
	if (InviteButton)
	{
		InviteButton->OnLobbyButtonClicked.RemoveDynamic(this, &ThisClass::OnInviteClicked);
	}

	for (int32 i = 0; i < CharacterSlots.Num(); ++i)
	{
		if (CharacterSlots[i])
		{
			CharacterSlots[i]->OnCharacterSelectClicked.RemoveDynamic(this, &UGRLobbyHUDWidget::OnCharacterSelected);
		}
	}
}

void UGRLobbyHUDWidget::UpdateLobbyButtonVisibility(bool bHost)
{
	if (!StartGameButton || !ReadyGameButton)
	{
		return;
	}

	if (bHost)
	{
		StartGameButton->SetVisibility(ESlateVisibility::Visible);
		ReadyGameButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		StartGameButton->SetVisibility(ESlateVisibility::Collapsed);
		ReadyGameButton->SetVisibility(ESlateVisibility::Visible);
	}
}

void UGRLobbyHUDWidget::OnCharacterSelected(int32 SelectedIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Character %d selected"), SelectedIndex);

	UGRLobbyCharacterSelectSlotWidget* ClickedSlot = nullptr;

	if (CharacterSlots.IsValidIndex(SelectedIndex))
	{
		ClickedSlot = CharacterSlots[SelectedIndex];
	}

	if (!ClickedSlot)
	{
		return;
	}

	if (CurrentClickedSlot && CurrentClickedSlot != ClickedSlot)
	{
		CurrentClickedSlot->GetSizeBox()->SetRenderOpacity(0.5f);
		CurrentClickedSlot->GetBorder()->SetBrushColor(FLinearColor::White);
		CurrentClickedSlot->bIsClicked = false;
	}

	ClickedSlot->GetSizeBox()->SetRenderOpacity(1.0f);
	ClickedSlot->GetBorder()->SetBrushColor(FLinearColor::Green);
	ClickedSlot->bIsClicked = true;
	CurrentClickedSlot = ClickedSlot;
	
	//캐릭터 선택 UI
}

void UGRLobbyHUDWidget::OnPlayerInfoClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Player info clicked"));
}

void UGRLobbyHUDWidget::OnPlayerPerksClicked()
{
	AGRLobbyPlayerController* LobbyPlayerController = GetOwningPlayer<AGRLobbyPlayerController>();
	if (!IsValid(LobbyPlayerController))
	{
		return;
	}
	LobbyPlayerController->HideLobbyWidget();
	LobbyPlayerController->ShowPerkWidget();
}

void UGRLobbyHUDWidget::OnStartGameClicked()
{
	// [NOTE] TODO: 준비 완료를 확인한 뒤에 게임을 시작해야 함
	// 테스트를 위해 바로 시작

	AGRLobbyPlayerController* LobbyPlayerController = GetOwningPlayer<AGRLobbyPlayerController>();
	if (!IsValid(LobbyPlayerController))
	{
		return;
	}
	LobbyPlayerController->StartGame();
}

void UGRLobbyHUDWidget::OnReadyGameClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Ready game clicked"));
}

void UGRLobbyHUDWidget::OnExitLobbyClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Exit Lobby clicked"));
}

void UGRLobbyHUDWidget::OnInviteClicked()
{
	if (!SteamFriendsList)
	{
		UE_LOG(LogTemp, Error, TEXT("SteamFriendsList is INVALID"));
		return;
	}

	ESlateVisibility ListVisibility = SteamFriendsList->GetVisibility();

	if (ListVisibility == ESlateVisibility::Collapsed)
	{
		SteamFriendsList->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SteamFriendsList->SetVisibility(ESlateVisibility::Collapsed);
	}
}
