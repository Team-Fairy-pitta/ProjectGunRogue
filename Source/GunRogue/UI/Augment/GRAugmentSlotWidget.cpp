// GRAugmentSlotWidget.cpp


#include "UI/Augment/GRAugmentSlotWidget.h"
#include "Augment/GRAugmentDefinition.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Player/Battle/GRBattlePlayerController.h"


void UGRAugmentSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SlotButton)
	{
		SlotButton->OnHovered.AddDynamic(this, &UGRAugmentSlotWidget::OnSlotHovered);
		SlotButton->OnUnhovered.AddDynamic(this, &UGRAugmentSlotWidget::OnSlotUnhovered);
		SlotButton->OnClicked.AddDynamic(this, &UGRAugmentSlotWidget::OnSlotClicked);
	}

	if (SlotBackground)
	{
		DefaultBrushColor = SlotBackground->GetBrushColor();
	}
}

void UGRAugmentSlotWidget::SetAugmentSlot(UGRAugmentDefinition* Augment, int32 Level)
{
	CurrentAugment = Augment;
	CurrentLevel = Level;
	
	UpdateAugmentSlotUI();
}

void UGRAugmentSlotWidget::UpdateAugmentSlotUI()
{
	if (!CurrentAugment)
	{
		return;
	}

	SetSlotName();
	SetSlotIcon();
	SetSlotLevelBar();
	SetSlotDescription();
	SetSlotCategory();
}

void UGRAugmentSlotWidget::SetSlotName()
{
	if (!SlotName)
	{
		return;
	}
	
	SlotName->SetText(CurrentAugment->AugmentName);
}

void UGRAugmentSlotWidget::SetSlotIcon()
{
	if (!SlotIcon)
	{
		return;
	}
	
	SlotIcon->SetBrushFromTexture(CurrentAugment->AugmentIcon);
}

void UGRAugmentSlotWidget::SetSlotLevelBar()
{
	if (!SlotLevelBar)
	{
		return;
	}
	
	float LevelPercent = (static_cast<float>(CurrentLevel) / 3);
	SlotLevelBar->SetPercent(LevelPercent);
}

void UGRAugmentSlotWidget::SetSlotDescription()
{
	if (!SlotDescription)
	{
		return;
	}

	TArray<FFormatArgumentValue> Args;
	for (const FAugmentValues& Val : CurrentAugment->AugmentValues)
	{
		Args.Add(FFormatArgumentValue(Val.ValuePerLevel[CurrentLevel]));
	}
	FText FinalDescription = FText::Format(CurrentAugment->AugmentDescription, Args);

	if ( CurrentLevel == 2 && CurrentAugment->AugmentAdditionalValues.Num() > 0)
	{
		for (const FAugmentAdditionalValue& AddVal : CurrentAugment->AugmentAdditionalValues)
		{
			FText AddText;
			if (AddVal.AugmentAdditionalValue != 0.f)
			{
				TArray<FFormatArgumentValue> AddArgs;
				AddArgs.Add(FFormatArgumentValue(AddVal.AugmentAdditionalValue));

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
	
	SlotDescription->SetText(FinalDescription);
}

void UGRAugmentSlotWidget::SetSlotCategory()
{
	if (!SlotCategory)
	{
		return;
	}
	
	SlotCategory->SetText(CurrentAugment->AugmentCategoryName);
}

void UGRAugmentSlotWidget::OnSlotHovered()
{
	OnAugmentSlotHovered.Broadcast(this);

	if (!SlotBackground)
	{
		return;
	}

	SlotBackground->SetBrushColor(HoveredBrushColor);
}

void UGRAugmentSlotWidget::OnSlotUnhovered()
{
	OnAugmentSlotUnhovered.Broadcast(this);

	if (!SlotBackground)
	{
		return;
	}

	SlotBackground->SetBrushColor(DefaultBrushColor);
}

void UGRAugmentSlotWidget::OnSlotClicked()
{
	OnAugmentSlotClicked.Broadcast(this);

	if (!CurrentAugment)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	AGRBattlePlayerController* GRPC = Cast<AGRBattlePlayerController>(PC);
	if (!GRPC)
	{
		return;
	}

	GRPC->RequestSelectAugment(CurrentAugment->AugmentID);
}
