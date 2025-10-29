#include "UI/KeyMapping/GRKeyMappingWidget.h"
#include "UI/KeyMapping/GRKeyMappingSlot.h"
#include "Components/Scrollbox.h"
#include "UserSettings/EnhancedInputUserSettings.h"

void UGRKeyMappingWidget::InitKeyMappings(TMap<FName, FPlayerKeyMapping>& Mappings)
{
	if (!ScrollBox)
	{
		return;
	}

	for (auto& MappingPair : Mappings)
	{
		const FName& ActionName = MappingPair.Key;
		const FPlayerKeyMapping& Mapping = MappingPair.Value;
		const FName& KeyName = Mapping.GetCurrentKey().GetFName();

		UGRKeyMappingSlot* NewSlot = CreateWidget<UGRKeyMappingSlot>(GetOwningPlayer(), SlotClass);

		if (NewSlot)
		{
			NewSlot->Init(ActionName, KeyName, this);
			ScrollBox->AddChild(NewSlot);
		}

	}
}

void UGRKeyMappingWidget::ClearWidgets()
{
	if (!ScrollBox)
	{
		return;
	}

	ScrollBox->ClearChildren();
}

void UGRKeyMappingWidget::StartChange(UGRKeyMappingSlot* Current)
{
	isChanging = true;
	CurrentChangingSlot = Current;
}

void UGRKeyMappingWidget::EndChange()
{
	isChanging = false;
	if (CurrentChangingSlot)
	{
		CurrentChangingSlot->UnfocusChangeButton();
	}
	CurrentChangingSlot = nullptr;
}


bool UGRKeyMappingWidget::IsChanging()
{
	return isChanging;
}
