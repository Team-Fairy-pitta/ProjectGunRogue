// GRLobbyCharacterSelectSlotWidget.cpp


#include "UI/TitleHUD/SubWidgets/GRLobbyCharacterSelectSlotWidget.h"

#include "Components/Button.h"
#include "UI/Common/GRBaseTextWidget.h"

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
