#include "UI/KeySetting/GRKeySettingSlot.h"
#include "UI/KeySetting/GRKeySettingWidget.h"
#include "Components/TextBlock.h"
#include "Components/InputKeySelector.h"

void UGRKeySettingSlot::NativeConstruct()
{
	if (!InputKeySelector)
	{
		UE_LOG(LogTemp, Error, TEXT("InputKeySelector is INVALID."));
		return;
	}

	InputKeySelector->OnKeySelected.AddDynamic(this, &ThisClass::OnKeySelected);
	InputKeySelector->OnIsSelectingKeyChanged.AddDynamic(this, &ThisClass::OnIsSelectingKeyChanged);
	bIsKeyChanged = false;
}

void UGRKeySettingSlot::SetMappingName(const FName& InName)
{
	MappingName = InName;
}

void UGRKeySettingSlot::SetActionText(const FText& InText)
{
	if (!ActionText)
	{
		UE_LOG(LogTemp, Error, TEXT("ActionText is INVALID."));
		return;
	}

	ActionText->SetText(InText);
}

void UGRKeySettingSlot::SetKey(const FKey& InKey)
{
	if (!InputKeySelector)
	{
		UE_LOG(LogTemp, Error, TEXT("InputKeySelector is INVALID."));
		return;
	}

	InputKeySelector->SetSelectedKey(FInputChord(InKey));
}

void UGRKeySettingSlot::SetParrentWidget(UGRKeySettingWidget* InParrentWidget)
{
	ParrentWidget = InParrentWidget;
}

void UGRKeySettingSlot::OnKeySelected(FInputChord SelectedKey)
{
	if (!bIsKeyChanged)
	{
		return;
	}

	bIsKeyChanged = false;

	if (ParrentWidget.IsValid())
	{
		UGRKeySettingWidget* Parent = ParrentWidget.Get();
		Parent->UpdateMappings(MappingName, SelectedKey.Key);
	}
}

void UGRKeySettingSlot::OnIsSelectingKeyChanged()
{
	bIsKeyChanged = true;
}
