// GRPerkSlotWidget.cpp


#include "UI/MetaProgression/GRPerkSlotWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "MetaProgression/GRPerkSubsystem.h"
#include "MetaProgression/PerkInfoRow.h"
#include "GRPerkTooltipWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UGRPerkSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PerkSlotButton)
	{
		PerkSlotButton->OnClicked.AddDynamic(this, &UGRPerkSlotWidget::OnPerkClicked);
		PerkSlotButton->OnHovered.AddDynamic(this, &UGRPerkSlotWidget::OnPerkHovered);
		PerkSlotButton->OnUnhovered.AddDynamic(this, &UGRPerkSlotWidget::OnPerkUnhovered);
	}
}

void UGRPerkSlotWidget::SetupSlot(FName InPerkID, UGRPerkSubsystem* InSubsystem)
{
	PerkID = InPerkID;
	PerkSubsystem = InSubsystem;

	UpdatePerkUI();
}

void UGRPerkSlotWidget::SetPerkSlotIcon(UTexture2D* NewIcon)
{
	if (!PerkSlotIcon || !NewIcon)
	{
		return;
	}

	PerkSlotIcon->SetBrushFromTexture(NewIcon);
}

void UGRPerkSlotWidget::SetPerkSlotBar(int32 CurrentLevel, int32 MaxLevel)
{
	if (!PerkSlotBar)
	{
		return;
	}

	float LevelPercent = (MaxLevel > 0.f) ? (static_cast<float>(CurrentLevel) / static_cast<float>(MaxLevel)) : 0.f;
	PerkSlotBar->SetPercent(LevelPercent);
}

void UGRPerkSlotWidget::OnPerkClicked()
{
	if (!PerkSubsystem)
	{
		return;
	}
	
	if (PerkSubsystem->TryUpgradePerk(PerkID, PerkTable))
	{
		OnPerkSlotClicked.Broadcast(this);
		UpdatePerkUI();
	}
}

void UGRPerkSlotWidget::OnPerkHovered()
{
	OnPerkSlotHovered.Broadcast(this);
}

void UGRPerkSlotWidget::OnPerkUnhovered()
{
	OnPerkSlotUnhovered.Broadcast(this);
}

void UGRPerkSlotWidget::UpdatePerkUI()
{
	if (!PerkTable)
	{
		return;
	}

	FPerkInfoRow* Row = PerkTable->FindRow<FPerkInfoRow>(PerkID, TEXT(""));
	if (!Row)
	{
		return;
	}

	SetPerkSlotIcon(Row->PerkIcon);

	int32 CurrentLevel = PerkSubsystem->GetPerkLevel(PerkID);
	UE_LOG(LogTemp, Warning, TEXT("CurrentLevel: %d, MaxLevel: %d"), CurrentLevel, Row->MaxLevel);
	SetPerkSlotBar(CurrentLevel, Row->MaxLevel);
}
