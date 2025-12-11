// GRLobbyHUDWidget.cpp


#include "UI/TitleHUD/GRLobbyHUDWidget.h"

#include "Components/Border.h"
#include "SubWidgets/GRLobbyBaseButtonWidget.h"
#include "UI/TitleHUD/SubWidgets/GRLobbyCharacterSelectSlotWidget.h"
#include "UI/SteamInvite/GRSteamFriendsList.h"
#include "Components/SizeBox.h"
#include "Player/Lobby/GRLobbyPlayerController.h"
#include "Character/GRCharacter.h"
#include "Character/GRPawnData.h"

void UGRLobbyHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CharacterSlots = {
		FirstCharacterSlot,
		SecondCharacterSlot,
		ThirdCharacterSlot,
		//[NOTE] 캐릭터 수가 더 필요하다면, 주석을 해제하고 사용
		//FourthCharacterSlot,
		//FifthCharacterSlot,
		//SixthCharacterSlot
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
	if (InviteButton)
	{
		InviteButton->OnLobbyButtonClicked.AddDynamic(this, &ThisClass::OnInviteClicked);
	}
	
	InitCharacterSelectButtons();

	APlayerController* PlayerController = GetOwningPlayer();
	if (!IsValid(PlayerController))
	{
		return;
	}
	
	// [NOTE] Dedicated Server에서는 게임을 시작할 수 없음
	bool bIsHost = PlayerController->GetNetMode() == ENetMode::NM_ListenServer;
	UpdateLobbyButtonVisibility(bIsHost);

	// [NOTE] 캐릭터를 선택하지 않으면 준비할 수 없으며, 시작할 수 없음
	if (bIsHost)
	{
		DisableStartButton();
	}
	else
	{
		DisableReadyButton();
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
	SetSelectedCharacterIndex(SelectedIndex);
	UpdateCharacterButtonWidget(SelectedIndex);
}

void UGRLobbyHUDWidget::SetSelectedCharacterIndex(int32 SelectedIndex)
{
	AGRLobbyPlayerController* LobbyPlayerController = GetOwningPlayer<AGRLobbyPlayerController>();
	if (!IsValid(LobbyPlayerController))
	{
		return;
	}

	LobbyPlayerController->ServerRPC_SelectCharacter(SelectedIndex);

	bool bIsHost = LobbyPlayerController->GetNetMode() == ENetMode::NM_ListenServer;
	if (bIsHost)
	{
		if (LobbyPlayerController->IsAllPlayerReady())
		{
			EnableStartButton();
		}
	}
	else
	{
		EnableReadyButton();
	}
}

void UGRLobbyHUDWidget::UpdateCharacterButtonWidget(int32 SelectedIndex)
{
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
}

void UGRLobbyHUDWidget::InitCharacterSelectButtons()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (!IsValid(PlayerController))
	{
		return;
	}

	AGRLobbyPlayerController* GRLobbyPlayerController = Cast<AGRLobbyPlayerController>(PlayerController);
	if (!IsValid(GRLobbyPlayerController))
	{
		return;
	}

	for (int32 i = 0; i < CharacterSlots.Num(); ++i)
	{
		UTexture2D* Thumbnail = nullptr;
		FText CharacterName;
		if (GRLobbyPlayerController->PlayableCharacterClasses.IsValidIndex(i))
		{
			TSubclassOf<AGRCharacter> CharacterClass = GRLobbyPlayerController->PlayableCharacterClasses[i];
			AGRCharacter* CDO = Cast<AGRCharacter>(CharacterClass->GetDefaultObject());
			if (CDO)
			{
				UGRPawnData* PawnData = CDO->PawnData;
				if (PawnData)
				{
					Thumbnail = PawnData->CharacterThumbnail;
					CharacterName = PawnData->CharacterName;
				}
			}
		}

		if (CharacterSlots[i])
		{
			CharacterSlots[i]->CharacterIndex = i;
			CharacterSlots[i]->OnCharacterSelectClicked.AddDynamic(this, &UGRLobbyHUDWidget::OnCharacterSelected);
			CharacterSlots[i]->SetCharacterImage(Thumbnail);
			CharacterSlots[i]->SetCharacterName(CharacterName);
		}
	}
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
	AGRLobbyPlayerController* LobbyPlayerController = GetOwningPlayer<AGRLobbyPlayerController>();
	if (!IsValid(LobbyPlayerController))
	{
		return;
	}

	if (LobbyPlayerController->GetNetMode() != ENetMode::NM_Client)
	{
		return;
	}

	if (!ReadyGameButton)
	{
		return;
	}

	// Toggle
	if (bIsReady)
	{
		bIsReady = false;
		DisableReadyButton();
		ReadyGameButton->SetButtonText(FText::FromString(TEXT("준비")));
		LobbyPlayerController->ServerRPC_CancelReady();
	}
	else
	{
		bIsReady = true;
		DisableButtonsOnReady();
		DisableReadyButton();
		ReadyGameButton->SetButtonText(FText::FromString(TEXT("준비중...")));
		LobbyPlayerController->ServerRPC_Ready();
	}
}

void UGRLobbyHUDWidget::DisableButtonsOnReady()
{
	TArray<UGRLobbyBaseButtonWidget*> Buttons = {
		PlayerInfoButton,
		PlayerPerksButton,
		ExitLobbyButton,
		InviteButton,
	};

	for (UGRLobbyBaseButtonWidget* Button : Buttons)
	{
		if (Button)
		{
			Button->DisableButton();
		}
	}

	for (int32 i = 0; i < CharacterSlots.Num(); ++i)
	{
		if (CharacterSlots[i])
		{
			CharacterSlots[i]->DisableCharacterButton();
		}
	}
}

void UGRLobbyHUDWidget::EnableButtons()
{
	TArray<UGRLobbyBaseButtonWidget*> Buttons = {
		PlayerInfoButton,
		PlayerPerksButton,
		ExitLobbyButton,
		InviteButton,
	};

	for (UGRLobbyBaseButtonWidget* Button : Buttons)
	{
		if (Button)
		{
			Button->EnableButton();
		}
	}

	for (int32 i = 0; i < CharacterSlots.Num(); ++i)
	{
		if (CharacterSlots[i])
		{
			CharacterSlots[i]->EnableCharacterButton();
		}
	}
}

void UGRLobbyHUDWidget::DisableReadyButton()
{
	if (ReadyGameButton)
	{
		ReadyGameButton->DisableButton();
	}
}

void UGRLobbyHUDWidget::EnableReadyButton()
{
	if (ReadyGameButton)
	{
		ReadyGameButton->EnableButton();
	}
}

void UGRLobbyHUDWidget::DisableStartButton()
{
	if (StartGameButton)
	{
		StartGameButton->DisableButton();
	}
}

void UGRLobbyHUDWidget::EnableStartButton()
{
	if (StartGameButton)
	{
		StartGameButton->EnableButton();
	}
}

void UGRLobbyHUDWidget::OnHostExit(const FString& URL)
{
	AGRLobbyPlayerController* LobbyPlayerController = GetOwningPlayer<AGRLobbyPlayerController>();
	if (!IsValid(LobbyPlayerController))
	{
		return;
	}

	if (LobbyPlayerController->HasAuthority())
	{
		LobbyPlayerController->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
	}
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
