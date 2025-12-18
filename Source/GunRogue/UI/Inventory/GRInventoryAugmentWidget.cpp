// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/GRInventoryAugmentWidget.h"

#include "GRInventoryAugmentSlot.h"
#include "UI/Augment/GRAugmentTooltipWidget.h"
#include "UI/Augment/GRAugmentSlotWidget.h"
#include "Augment/GRAugmentSubsystem.h"
#include "Augment/GRAugmentDefinition.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Player/GRPlayerState.h"

void UGRInventoryAugmentWidget::NativeConstruct()
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

void UGRInventoryAugmentWidget::NativeDestruct()
{
	if (AugmentTooltipWidget)
	{
		AugmentTooltipWidget->RemoveFromParent();
		AugmentTooltipWidget = nullptr;
	}
	
	if (AugmentContainer)
	{
		for (UWidget* Child : AugmentContainer->GetAllChildren())
		{
			if (UGRInventoryAugmentSlot* AugmentSlot = Cast<UGRInventoryAugmentSlot>(Child))
			{
				AugmentSlot->OnInventoryAugmentSlotHovered.RemoveDynamic(this, &UGRInventoryAugmentWidget::ShowTooltip);
				AugmentSlot->OnInventoryAugmentSlotUnhovered.RemoveDynamic(this, &UGRInventoryAugmentWidget::HideTooltip);

				AugmentSlot->RemoveFromParent();
			}
		}
	}
	
	Super::NativeDestruct();
}

void UGRInventoryAugmentWidget::UpdateTooltip(UGRInventoryAugmentSlot* AugmentSlot)
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

	AGRPlayerState* PS = PC->GetPlayerState<AGRPlayerState>();
	if (!PS)
	{
		return;
	}

	UGRAugmentDefinition* SlotAugment = AugmentSlot->GetCurrentAugment();
	int32 SlotLevel = PS->GetAugmentLevel(SlotAugment->AugmentID);

	AugmentTooltipWidget->SetAugmentTooltip(SlotAugment, SlotLevel-1);
}

void UGRInventoryAugmentWidget::ShowTooltip(UGRInventoryAugmentSlot* AugmentSlot)
{
	if (!AugmentTooltipWidget)
	{
		return;
	}

	UpdateTooltip(AugmentSlot);

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

void UGRInventoryAugmentWidget::HideTooltip(UGRInventoryAugmentSlot* AugmentSlot)
{
	if (!AugmentTooltipWidget)
	{
		return;
	}

	AugmentTooltipWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UGRInventoryAugmentWidget::CreateAugmentSlot()
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

	AGRPlayerState* PS = PC->GetPlayerState<AGRPlayerState>();
	if (!PS)
	{
		return;
	}
	
	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		return;
	}

	UGRAugmentSubsystem* AugmentSubsystem = GI->GetSubsystem<UGRAugmentSubsystem>();
	if (!AugmentSubsystem)
	{
		return;
	}

	FName CharacterType = TEXT("Bomb"); //테스트용 원래는 따로 CharacterType을 받아야함
	TArray<UGRAugmentDefinition*> AugmentValue = AugmentSubsystem->GetAugmentValues(CharacterType);

	for (UGRAugmentDefinition* Augment : AugmentValue)
	{
		if (!Augment)
		{
			continue;
		}

		int32 CurrentLevel = PS->GetAugmentLevel(Augment->AugmentID);

		UE_LOG(LogTemp,Warning,TEXT("이름 : %s 레벨 : %d"),*Augment->AugmentName.ToString(),CurrentLevel);
		
		UGRInventoryAugmentSlot* NewAugmentSlot = CreateWidget<UGRInventoryAugmentSlot>(PC, AugmentSlotClass);
		if (!NewAugmentSlot)
		{
			continue;
		}
		
		AugmentContainer->AddChildToWrapBox(NewAugmentSlot);
		
		
		
		if (CurrentLevel > 0)
		{
			NewAugmentSlot->SetAugmentSlot(Augment, CurrentLevel);
			NewAugmentSlot->OnInventoryAugmentSlotHovered.AddDynamic(this, &UGRInventoryAugmentWidget::ShowTooltip);
			NewAugmentSlot->OnInventoryAugmentSlotUnhovered.AddDynamic(this, &UGRInventoryAugmentWidget::HideTooltip);
		}
		else
		{
			NewAugmentSlot->SetAugmentSlot(Augment, CurrentLevel);
			NewAugmentSlot->SetRenderOpacity(0.5f);
		}


	}
}
