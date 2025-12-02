// GRAugmentHUDWidget.cpp


#include "UI/Augment/GRAugmentHUDWidget.h"

#include "GRAugmentTooltipWidget.h"
#include "GRAugmentSlotWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

void UGRAugmentHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateAugmentSlot();

	if (AugmentTooltipClass)
	{
		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			AugmentTooltipWidget = CreateWidget<UGRAugmentTooltipWidget>(PC, AugmentTooltipClass);
			if (AugmentTooltipWidget)
			{
				AugmentTooltipWidget->AddToViewport(2);
				AugmentTooltipWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void UGRAugmentHUDWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (AugmentContainer)
	{
		for (UWidget* Child : AugmentContainer->GetAllChildren())
		{
			if (UGRAugmentSlotWidget* AugmentSlot = Cast<UGRAugmentSlotWidget>(Child))
			{
				AugmentSlot->OnAugmentSlotHovered.RemoveDynamic(this, &UGRAugmentHUDWidget::ShowTooltip);
				AugmentSlot->OnAugmentSlotUnhovered.RemoveDynamic(this, &UGRAugmentHUDWidget::HideTooltip);
				AugmentSlot->OnAugmentSlotClicked.RemoveDynamic(this, &UGRAugmentHUDWidget::RemoveAugmentHUD);

				AugmentSlot->RemoveFromParent();
			}
		}
	}
}

void UGRAugmentHUDWidget::ShowTooltip(UGRAugmentSlotWidget* AugmentSlot)
{
	if (!AugmentTooltipWidget)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	FVector2D MousePos;
	PC->GetMousePosition(MousePos.X, MousePos.Y);

	AugmentTooltipWidget->SetPositionInViewport(MousePos + FVector2D(10, 10), true);
	AugmentTooltipWidget->SetVisibility(ESlateVisibility::Visible);
}

void UGRAugmentHUDWidget::HideTooltip(UGRAugmentSlotWidget* AugmentSlot)
{
	if (!AugmentTooltipWidget)
	{
		return;
	}

	AugmentTooltipWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UGRAugmentHUDWidget::RemoveAugmentHUD(UGRAugmentSlotWidget* AugmentSlot)
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	FInputModeGameOnly GameMode;
	PC->SetInputMode(GameMode);
	PC->bShowMouseCursor = false;
	
	if (AugmentTooltipWidget)
	{
		AugmentTooltipWidget->RemoveFromParent();
		AugmentTooltipWidget = nullptr;
	}

	RemoveFromParent();
}

void UGRAugmentHUDWidget::CreateAugmentSlot()
{
	if (!AugmentSlotClass || !AugmentContainer)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	for (int32 i = 0; i < 3; i++)
	{
		UGRAugmentSlotWidget* NewAugmentSlot = CreateWidget<UGRAugmentSlotWidget>(PC, AugmentSlotClass);
		if (!NewAugmentSlot)
		{
			continue;
		}

		if (UHorizontalBoxSlot* HorizontalBoxSlot = AugmentContainer->AddChildToHorizontalBox(NewAugmentSlot))
		{
			HorizontalBoxSlot->SetPadding(FMargin(0,0,20, 0));
		}

		NewAugmentSlot->OnAugmentSlotHovered.AddDynamic(this, &UGRAugmentHUDWidget::ShowTooltip);
		NewAugmentSlot->OnAugmentSlotUnhovered.AddDynamic(this, &UGRAugmentHUDWidget::HideTooltip);
		NewAugmentSlot->OnAugmentSlotClicked.AddDynamic(this, &UGRAugmentHUDWidget::RemoveAugmentHUD);
	}
}
