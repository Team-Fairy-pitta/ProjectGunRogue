// GRAugmentSlotWidget.cpp


#include "UI/Augment/GRAugmentSlotWidget.h"

#include "Components/Button.h"
#include "Components/Border.h"


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

void UGRAugmentSlotWidget::OnSlotHovered()
{
	OnAugmentSlotHovered.Broadcast(this);

	if (!SlotBackground)
	{
		return;
	}

	SlotBackground->SetBrushColor(FLinearColor::Green);
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
}
