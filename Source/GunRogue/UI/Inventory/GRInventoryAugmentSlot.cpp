// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/GRInventoryAugmentSlot.h"
#include "Augment/GRAugmentDefinition.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Player/Battle/GRBattlePlayerController.h"

void UGRInventoryAugmentSlot::SetAugmentSlot(UGRAugmentDefinition* Augment, int32 Level)
{
	CurrentAugment = Augment;
	CurrentLevel = Level;
	
	UpdateAugmentSlotUI();
}

void UGRInventoryAugmentSlot::UpdateAugmentSlotUI()
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

void UGRInventoryAugmentSlot::SetSlotName()
{
	if (!SlotName)
	{
		return;
	}
	
	SlotName->SetText(CurrentAugment->AugmentName);
}

void UGRInventoryAugmentSlot::SetSlotIcon()
{
	if (!SlotIcon)
	{
		return;
	}
	
	SlotIcon->SetBrushFromTexture(CurrentAugment->AugmentIcon);
}

void UGRInventoryAugmentSlot::SetSlotLevelBar()
{
	if (!SlotLevelBar)
	{
		return;
	}
	
	float LevelPercent = (static_cast<float>(CurrentLevel) / 3);
	SlotLevelBar->SetPercent(LevelPercent);
}

void UGRInventoryAugmentSlot::SetSlotDescription()
{
	if (!SlotDescription)
	{
		return;
	}

	if (CurrentLevel > 0)
	{
		CurrentLevel--;
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

void UGRInventoryAugmentSlot::SetSlotCategory()
{
	if (!SlotCategory)
	{
		return;
	}
	
	SlotCategory->SetText(CurrentAugment->AugmentCategoryName);
}

void UGRInventoryAugmentSlot::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (SlotButton)
	{
		SlotButton->OnHovered.AddDynamic(this, &UGRInventoryAugmentSlot::OnSlotHovered);
		SlotButton->OnUnhovered.AddDynamic(this, &UGRInventoryAugmentSlot::OnSlotUnhovered);
	}

	if (SlotBackground)
	{
		DefaultBrushColor = SlotBackground->GetBrushColor();
	}
	
}

void UGRInventoryAugmentSlot::OnSlotHovered()
{
	OnInventoryAugmentSlotHovered.Broadcast(this);

	if (!SlotBackground)
	{
		return;
	}

	SlotBackground->SetBrushColor(HoveredBrushColor);
}

void UGRInventoryAugmentSlot::OnSlotUnhovered()
{
	OnInventoryAugmentSlotUnhovered.Broadcast(this);

	if (!SlotBackground)
	{
		return;
	}

	SlotBackground->SetBrushColor(DefaultBrushColor);
}
