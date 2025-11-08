#include "UI/KeySetting/GRKeySettingSlot.h"
#include "UI/KeySetting/GRKeySettingWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
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

	if (!DefaultButton)
	{
		UE_LOG(LogTemp, Error, TEXT("DefaultButton is INVALID."));
		return;
	}
	DefaultButton->OnClicked.AddDynamic(this, &ThisClass::OnDefaultButtonClicked);

	if (!ClearButton)
	{
		UE_LOG(LogTemp, Error, TEXT("ClearButton is INVALID."));
		return;
	}
	ClearButton->OnClicked.AddDynamic(this, &ThisClass::OnClearButtonClicked);
}

void UGRKeySettingSlot::SetKeyMapping(const FPlayerKeyMapping& InMapping)
{
	Mapping = InMapping;

	SetMappingName(Mapping.GetMappingName());
	SetActionText(Mapping.GetDisplayName());
	SetKey(Mapping.GetCurrentKey());

	SetDefaultButtonVisibility(Mapping.GetCurrentKey());
}

void UGRKeySettingSlot::SetParrentWidget(UGRKeySettingWidget* InParrentWidget)
{
	ParrentWidget = InParrentWidget;
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

void UGRKeySettingSlot::SetDefaultButtonVisibility(const FKey& InCurrentKey)
{
	if (!DefaultButton)
	{
		UE_LOG(LogTemp, Error, TEXT("DefaultButton (UButton) is INVALID."));
		return;
	}

	if (InCurrentKey == Mapping.GetDefaultKey())
	{
		DefaultButton->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		DefaultButton->SetVisibility(ESlateVisibility::Visible);
	}
}

void UGRKeySettingSlot::NotifyToParent(const FKey& NewKey)
{
	if (ParrentWidget.IsValid())
	{
		UGRKeySettingWidget* Parent = ParrentWidget.Get();
		Parent->UpdateMappings(MappingName, NewKey);
	}
}

void UGRKeySettingSlot::OnKeySelected(FInputChord SelectedKey)
{
	if (!bIsKeyChanged)
	{
		return;
	}

	bIsKeyChanged = false;
	NotifyToParent(SelectedKey.Key);
	SetDefaultButtonVisibility(SelectedKey.Key);
}

void UGRKeySettingSlot::OnIsSelectingKeyChanged()
{
	bIsKeyChanged = true;
}

void UGRKeySettingSlot::OnDefaultButtonClicked()
{
	if (!InputKeySelector)
	{
		UE_LOG(LogTemp, Error, TEXT("InputKeySelector (UInputKeySelector) is INVALID."));
		return;
	}

	const FKey& DefaultKey = Mapping.GetDefaultKey();
	InputKeySelector->SetSelectedKey(FInputChord(DefaultKey));
	NotifyToParent(DefaultKey);
	SetDefaultButtonVisibility(DefaultKey);
}

void UGRKeySettingSlot::OnClearButtonClicked()
{
	if (!InputKeySelector)
	{
		UE_LOG(LogTemp, Error, TEXT("InputKeySelector (UInputKeySelector) is INVALID."));
		return;
	}

	const FKey& NoneKey = FKey();
	InputKeySelector->SetSelectedKey(FInputChord(NoneKey));
	NotifyToParent(NoneKey);
	SetDefaultButtonVisibility(NoneKey);
}
