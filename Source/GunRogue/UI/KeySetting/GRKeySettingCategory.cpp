#include "UI/KeySetting/GRKeySettingCategory.h"
#include "UI/KeySetting/GRKeySettingSlot.h"
#include "UI/KeySetting/GRKeySettingWidget.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UGRKeySettingCategory::SetCategoryText(const FText& InText)
{
	if (!CategoryText)
	{
		UE_LOG(LogTemp, Error, TEXT("CategoryText is INVALID."));
		return;
	}
	
	CategoryText->SetText(InText);
}

void UGRKeySettingCategory::AddMappings(const TArray<FPlayerKeyMapping>& InMappings, UGRKeySettingWidget* InParentWidget)
{
	if (!SlotWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SlotWidgetClass is INVALID."));
		return;
	}

	if (!MappingsVerticalBox)
	{
		UE_LOG(LogTemp, Error, TEXT("MappingsVerticalBox is INVALID."));
		return;
	}

	for (const FPlayerKeyMapping& Mapping : InMappings)
	{
		UGRKeySettingSlot* NewSlot = CreateWidget<UGRKeySettingSlot>(GetOwningPlayer(), SlotWidgetClass);
		if (NewSlot)
		{
			NewSlot->SetKeyMapping(Mapping);
			NewSlot->SetParrentWidget(InParentWidget);

			MappingsVerticalBox->AddChild(NewSlot);
		}
	}

	
}
