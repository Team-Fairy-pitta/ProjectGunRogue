// GRLobbyHUDWidget.cpp


#include "UI/TitleHUD/GRLobbyHUDWidget.h"

#include "Components/Border.h"
#include "SubWidgets/GRLobbyBaseButtonWidget.h"
#include "UI/TitleHUD/SubWidgets/GRLobbyCharacterSelectSlotWidget.h"
#include "Components/SizeBox.h"

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
		PlayerInfoButton->OnLobbyButtonClicked.AddDynamic(this, &UGRLobbyHUDWidget::OnPlayerInfoClicked);
	}
	if (PlayerPerksButton)
	{
		PlayerPerksButton->OnLobbyButtonClicked.AddDynamic(this, &UGRLobbyHUDWidget::OnPlayerPerksClicked);
	}
	if (StartGameButton)
	{
		StartGameButton->SetVisibility(ESlateVisibility::Visible);
		StartGameButton->OnLobbyButtonClicked.AddDynamic(this, &UGRLobbyHUDWidget::OnStartGameClicked);
	}
	if (ReadyGameButton)
	{
		ReadyGameButton->SetVisibility(ESlateVisibility::Collapsed);
		ReadyGameButton->OnLobbyButtonClicked.AddDynamic(this, &UGRLobbyHUDWidget::OnReadyGameClicked);
	}
	if (ExitLobbyButton)
	{
		ExitLobbyButton->OnLobbyButtonClicked.AddDynamic(this, &UGRLobbyHUDWidget::OnExitLobbyClicked);
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
	UE_LOG(LogTemp, Warning, TEXT("Player perks clicked"));
}

void UGRLobbyHUDWidget::OnStartGameClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Start game clicked"));
}

void UGRLobbyHUDWidget::OnReadyGameClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Ready game clicked"));
}

void UGRLobbyHUDWidget::OnExitLobbyClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Exit Lobby clicked"));
}
