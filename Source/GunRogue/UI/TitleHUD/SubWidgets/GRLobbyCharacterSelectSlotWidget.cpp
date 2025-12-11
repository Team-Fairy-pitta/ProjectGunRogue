// GRLobbyCharacterSelectSlotWidget.cpp


#include "UI/TitleHUD/SubWidgets/GRLobbyCharacterSelectSlotWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UGRLobbyCharacterSelectSlotWidget::SetCharacterImage(UTexture2D* InTexture)
{
	if (CharImage)
	{
		CharImage->SetBrushFromTexture(InTexture);
	}
}

void UGRLobbyCharacterSelectSlotWidget::SetCharacterName(const FText& InCharacterName)
{
	if (CharNameText)
	{
		CharNameText->SetText(InCharacterName);
	}
}

void UGRLobbyCharacterSelectSlotWidget::EnableCharacterButton()
{
	if (CharSelectButton)
	{
		CharSelectButton->SetIsEnabled(true);
	}
}


void UGRLobbyCharacterSelectSlotWidget::DisableCharacterButton()
{
	if (CharSelectButton)
	{
		CharSelectButton->SetIsEnabled(false);
	}
}

void UGRLobbyCharacterSelectSlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (CharNameText)
	{
		CharNameText->SetText(DefaultText);
	}
}

void UGRLobbyCharacterSelectSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CharSelectButton)
	{
		CharSelectButton->OnClicked.AddDynamic(this, &UGRLobbyCharacterSelectSlotWidget::OnCharSelectClicked);
	}
}

void UGRLobbyCharacterSelectSlotWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (CharSelectButton)
	{
		CharSelectButton->OnClicked.RemoveDynamic(this, &UGRLobbyCharacterSelectSlotWidget::OnCharSelectClicked);
	}
}

void UGRLobbyCharacterSelectSlotWidget::OnCharSelectClicked()
{
	OnCharacterSelectClicked.Broadcast(CharacterIndex);
}
