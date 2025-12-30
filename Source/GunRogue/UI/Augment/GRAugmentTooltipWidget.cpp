// GRAugmentTooltipWidget.cpp


#include "UI/Augment/GRAugmentTooltipWidget.h"
#include "Augment/GRAugmentStructs.h"
#include "Components/TextBlock.h"
#include "Augment/GRAugmentDefinition.h"

void UGRAugmentTooltipWidget::NativeConstruct()
{
	Super::NativeConstruct();

	LevelDescriptions = {
		Level1Description,
		Level2Description,
		Level3Description
	};

	LevelTexts = {
		Level1Text,
		Level2Text,
		Level3Text
	};
}

void UGRAugmentTooltipWidget::SetAugmentTooltip(UGRAugmentDefinition* Augment, int32 Level)
{
	CurrentAugment = Augment;
	CurrentLevel = Level;

	UpdateAugmentTooltipUI();
}

void UGRAugmentTooltipWidget::UpdateAugmentTooltipUI()
{
	if (!CurrentAugment)
	{
		return;
	}
	
	for (int32 i = 0; i < LevelDescriptions.Num(); i++)
	{
		FText FinalDescription = SetLevelDescriptionText(i);
		LevelDescriptions[i]->SetText(FinalDescription);

		FLinearColor Color = SetLevelColors(i==CurrentLevel);
		LevelDescriptions[i]->SetColorAndOpacity(Color);
		LevelTexts[i]->SetColorAndOpacity(Color);
	}
}

FText UGRAugmentTooltipWidget::SetLevelDescriptionText(int32 Index)
{
	TArray<FFormatArgumentValue> Args;
	for (const FAugmentValues& Val : CurrentAugment->AugmentValues)
	{
		if (!Val.ValuePerLevel.IsValidIndex(Index))
		{
			continue;
		}
		
		float DisplayValue = Val.ValuePerLevel[Index];

		if (Val.bDisplayAsPercent)
		{
			DisplayValue *= 100.f;
		}
		
		Args.Add(FFormatArgumentValue(DisplayValue));
	}
	
	FText FinalDescription = FText::Format(CurrentAugment->AugmentDescription, Args);
	
	if ( Index == 2 && CurrentAugment->AugmentAdditionalValues.Num() > 0)
	{
		for (const FAugmentAdditionalValue& AddVal : CurrentAugment->AugmentAdditionalValues)
		{
			FText AddText;
			if (AddVal.AugmentAdditionalValue != 0.f)
			{
				float DisplayValue = AddVal.AugmentAdditionalValue;

				if (AddVal.bDisplayAsPercent)
				{
					DisplayValue *= 100.f;
				}
				
				TArray<FFormatArgumentValue> AddArgs;
				AddArgs.Add(FFormatArgumentValue(DisplayValue));

				AddText = FText::Format(AddVal.AugmentAdditionalDescription, AddArgs);

			}
			else
			{
				AddText = AddVal.AugmentAdditionalDescription;
			}
				
			FString Combined = FinalDescription.ToString();
			Combined.Append(AddText.ToString());

			FinalDescription = FText::FromString(Combined);
		}
	}

	return FinalDescription;
}

FLinearColor UGRAugmentTooltipWidget::SetLevelColors(bool bIsCurrentLevel)
{
	const FLinearColor Color = bIsCurrentLevel ? FLinearColor(CurrentLevelColor) : FLinearColor(FLinearColor::White);

	return Color;
}


