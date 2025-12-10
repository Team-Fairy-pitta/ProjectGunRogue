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

void UGRLobbyCharacterSelectSlotWidget::OnCharSelectClicked()
{
	OnCharacterSelectClicked.Broadcast(CharacterIndex);
}
