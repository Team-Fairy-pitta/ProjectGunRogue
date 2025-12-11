// GRPerkListWidget.cpp


#include "UI/MetaProgression/GRPerkListWidget.h"

#include "GRPerkHUDWidget.h"
#include "GRPerkSlotWidget.h"
#include "MetaProgression/PerkInfoRow.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "MetaProgression/GRPerkSubsystem.h"

void UGRPerkListWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (PerkSlotContainer)
	{
		for (UWidget* Child : PerkSlotContainer->GetAllChildren())
		{
			if (UGRPerkSlotWidget* PerkSlot = Cast<UGRPerkSlotWidget>(Child))
			{
				PerkSlot->OnPerkSlotHovered.RemoveDynamic(PerkHUD, &UGRPerkHUDWidget::ShowTooltipForSlot);
				PerkSlot->OnPerkSlotUnhovered.RemoveDynamic(PerkHUD, &UGRPerkHUDWidget::HideTooltipForSlot);
				PerkSlot->OnPerkSlotClicked.RemoveDynamic(PerkHUD, &UGRPerkHUDWidget::UpdateHUDAndTooltip);
				
				PerkSlot->RemoveFromParent();
			}
		}
	}
}

void UGRPerkListWidget::SetupList(UGRPerkHUDWidget* InHUD)
{
	PerkHUD = InHUD;
}

void UGRPerkListWidget::SetPerkCategoryText(FName InCategory)
{
	if (!PerkCategoryText)
	{
		return;
	}

	if (InCategory == FName("Battle"))
	{
		PerkCategoryText->SetText(FText::FromString(TEXT("전투")));
	}
	else if (InCategory == FName("Weapon"))
	{
		PerkCategoryText->SetText(FText::FromString(TEXT("무기")));
	}
	else if (InCategory == FName("Skill"))
	{
		PerkCategoryText->SetText(FText::FromString(TEXT("스킬")));
	}
	else if (InCategory == FName("SubSkill"))
	{
		PerkCategoryText->SetText(FText::FromString((TEXT("보조"))));
	}
}

void UGRPerkListWidget::CreateAllSlot(FName InCategory)
{
	if (!PerkSlotClass || !PerkSlotContainer)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	UGRPerkSubsystem* PerkSubsystem = GetGameInstance()->GetSubsystem<UGRPerkSubsystem>();
	if (!PerkSubsystem)
	{
		return;
	}

	UDataTable* PerkTable = PerkSubsystem->GetPerkTable();
	if (!PerkTable)
	{
		return;
	}

	TArray<FName> RowNames = PerkTable->GetRowNames();

	for (const FName& PerkID : RowNames)
	{
		FPerkInfoRow* Row = PerkTable->FindRow<FPerkInfoRow>(PerkID, TEXT(""));
		if (!Row)
		{
			continue;
		}

		if (Row->PerkCategory != InCategory)
		{
			continue;
		}

		UGRPerkSlotWidget* NewPerkSlot = CreateWidget<UGRPerkSlotWidget>(PC, PerkSlotClass);
		if (!NewPerkSlot)
		{
			continue;
		}

		if (UHorizontalBoxSlot* HorizontalBoxSlot = PerkSlotContainer->AddChildToHorizontalBox(NewPerkSlot))
		{
			HorizontalBoxSlot->SetPadding(FMargin(0,0,20,0));
			HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Left);
		}

		NewPerkSlot->SetupSlot(PerkID);
		NewPerkSlot->OnPerkSlotHovered.AddDynamic(PerkHUD, &UGRPerkHUDWidget::ShowTooltipForSlot);
		NewPerkSlot->OnPerkSlotClicked.AddDynamic(PerkHUD, &UGRPerkHUDWidget::UpdateHUDAndTooltip);
		NewPerkSlot->OnPerkSlotUnhovered.AddDynamic(PerkHUD, &UGRPerkHUDWidget::HideTooltipForSlot);

		// [NOTE] 실시간 반영을 원한다면, `OnPerkSlotClicked` Delegate에 특정 함수를 연결해야 함
	}
}
