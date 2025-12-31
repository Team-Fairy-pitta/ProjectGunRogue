// GRAugmentHUDWidget.cpp


#include "UI/Augment/GRAugmentHUDWidget.h"

#include "GRAugmentTooltipWidget.h"
#include "GRAugmentSlotWidget.h"
#include "Augment/GRAugmentSubsystem.h"
#include "Augment/GRAugmentDefinition.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Player/GRPlayerState.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "Character/GRCharacter.h"
#include "Character/GRPawnData.h"

void UGRAugmentHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateAugmentSlot();

	SetCharacterName();

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

	if (AugmentTooltipWidget)
	{
		AugmentTooltipWidget->RemoveFromParent();
		AugmentTooltipWidget = nullptr;
	}
	
	if (AugmentContainer)
	{
		for (UWidget* Child : AugmentContainer->GetAllChildren())
		{
			if (UGRAugmentSlotWidget* AugmentSlot = Cast<UGRAugmentSlotWidget>(Child))
			{
				AugmentSlot->OnAugmentSlotHovered.RemoveDynamic(this, &UGRAugmentHUDWidget::ShowTooltip);
				AugmentSlot->OnAugmentSlotUnhovered.RemoveDynamic(this, &UGRAugmentHUDWidget::HideTooltip);

				AugmentSlot->RemoveFromParent();
			}
		}
	}
}

void UGRAugmentHUDWidget::SetCharacterName()
{
	if (!CharacterName)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
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

	ACharacter* CurrentCharacter = PC->GetCharacter();
	if (!CurrentCharacter)
	{
		return;
	}
	
	AGRCharacter* GRCharacter = Cast<AGRCharacter>(CurrentCharacter);
	if (!GRCharacter)
	{
		return;
	}

	if (!GRCharacter->GetPawnData())
	{
		return;
	}

	FText CharacterNameText = GRCharacter->GetPawnData()->CharacterName;

	CharacterName->SetText(CharacterNameText);
}

void UGRAugmentHUDWidget::UpdateTooltip(UGRAugmentSlotWidget* AugmentSlot)
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

	AugmentTooltipWidget->SetAugmentTooltip(SlotAugment, SlotLevel);
}

void UGRAugmentHUDWidget::ShowTooltip(UGRAugmentSlotWidget* AugmentSlot)
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

void UGRAugmentHUDWidget::HideTooltip(UGRAugmentSlotWidget* AugmentSlot)
{
	if (!AugmentTooltipWidget)
	{
		return;
	}

	AugmentTooltipWidget->SetVisibility(ESlateVisibility::Hidden);
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

	ACharacter* CurrentCharacter = PC->GetCharacter();
	if (!CurrentCharacter)
	{
		return;
	}

	AGRCharacter* GRCharacter = Cast<AGRCharacter>(CurrentCharacter);
	if (!GRCharacter)
	{
		return;
	}

	if (!GRCharacter->GetPawnData())
	{
		return;
	}

	FName CharacterID = GRCharacter->GetPawnData()->CharacterID;

	UE_LOG(LogTemp, Display, TEXT("CharacterID: %s"), *CharacterID.ToString());
	
	TArray<UGRAugmentDefinition*> RandomAugments = AugmentSubsystem->GetRandomAugments(CharacterID, 3, PS);

	for (UGRAugmentDefinition* Augment : RandomAugments)
	{
		if (!Augment)
		{
			continue;
		}

		int32 CurrentLevel = PS->GetAugmentLevel(Augment->AugmentID);
		
		UGRAugmentSlotWidget* NewAugmentSlot = CreateWidget<UGRAugmentSlotWidget>(PC, AugmentSlotClass);
		if (!NewAugmentSlot)
		{
			continue;
		}

		if (UHorizontalBoxSlot* HorizontalBoxSlot = AugmentContainer->AddChildToHorizontalBox(NewAugmentSlot))
		{
			HorizontalBoxSlot->SetPadding(FMargin(0,0,20, 0));
		}

		NewAugmentSlot->SetAugmentSlot(Augment, CurrentLevel);
		NewAugmentSlot->OnAugmentSlotHovered.AddDynamic(this, &UGRAugmentHUDWidget::ShowTooltip);
		NewAugmentSlot->OnAugmentSlotUnhovered.AddDynamic(this, &UGRAugmentHUDWidget::HideTooltip);
	}
}
