// GRPerkSlotWidget.cpp


#include "UI/MetaProgression/GRPerkSlotWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "MetaProgression/GRPerkSubsystem.h"
#include "MetaProgression/PerkInfoRow.h"
#include "GRPerkTooltipWidget.h"
#include "Player/Lobby/GRLobbyPlayerController.h"
#include "Player/Lobby/GRLobbyPlayerState.h"

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

void UGRPerkSlotWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (PerkSlotButton)
	{
		PerkSlotButton->OnClicked.RemoveDynamic(this, &UGRPerkSlotWidget::OnPerkClicked);
		PerkSlotButton->OnHovered.RemoveDynamic(this, &UGRPerkSlotWidget::OnPerkHovered);
		PerkSlotButton->OnUnhovered.RemoveDynamic(this, &UGRPerkSlotWidget::OnPerkUnhovered);
	}
}

void UGRPerkSlotWidget::SetupSlot(FName InPerkID)
{
	PerkID = InPerkID;

	UpdatePerkSlot();
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
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	AGRLobbyPlayerState* PS = PC->GetPlayerState<AGRLobbyPlayerState>();
	if (!PS)
	{
		return;
	}

	if (PS->TryUpgradePerk(PerkID))
	{
		OnPerkSlotClicked.Broadcast(this);
		UpdatePerkSlot();
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

void UGRPerkSlotWidget::UpdatePerkSlot()
{
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

	FPerkInfoRow* Row = PerkTable->FindRow<FPerkInfoRow>(PerkID, TEXT(""));
	if (!Row)
	{
		return;
	}

	SetPerkSlotIcon(Row->PerkIcon);

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	AGRLobbyPlayerState* PS = PC->GetPlayerState<AGRLobbyPlayerState>();
	if (!PS)
	{
		return;
	}
	
	int32 CurrentLevel = PS->GetPerkLevel(PerkID);
	UE_LOG(LogTemp, Warning, TEXT("CurrentLevel: %d, MaxLevel: %d"), CurrentLevel, Row->MaxLevel);
	SetPerkSlotBar(CurrentLevel, Row->MaxLevel);
}
