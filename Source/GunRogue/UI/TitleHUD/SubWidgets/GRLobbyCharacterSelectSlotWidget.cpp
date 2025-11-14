// GRLobbyCharacterSelectSlotWidget.cpp


#include "UI/TitleHUD/SubWidgets/GRLobbyCharacterSelectSlotWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

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
