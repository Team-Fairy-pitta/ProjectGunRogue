#include "UI/KeyMapping/GRKeyMappingSlot.h"
#include "Components/TextBlock.h"

void UGRKeyMappingSlot::Init(const FName& ActionName, const FName& KeyName)
{
	if (ActionNameText)
	{
		ActionNameText->SetText(FText::FromName(ActionName));
	}
	if (KeyNameText)
	{
		KeyNameText->SetText(FText::FromName(KeyName));
	}
}
