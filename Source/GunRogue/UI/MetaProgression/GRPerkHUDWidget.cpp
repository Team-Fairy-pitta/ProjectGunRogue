// GRPerkHUDWidget.cpp


#include "UI/MetaProgression/GRPerkHUDWidget.h"

#include "MetaProgression/GRPerkSubsystem.h"
#include "UI/BattleHUD/SubWidgets/GRGoodsTextWidget.h"
#include "GRPerkListWidget.h"
#include "GRPerkSlotWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "GRPerkTooltipWidget.h"
#include "MetaProgression/PerkInfoRow.h"
#include "Components/Button.h"
#include "Player/Lobby/GRLobbyPlayerController.h"
#include "Player/Lobby/GRLobbyPlayerState.h"

void UGRPerkHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	UpdateGoodsText();
	CreatePerkList();

	APlayerController* PC = GetOwningPlayer();
	
	if (PerkTooltipClass)
	{
		if (PC)
		{
			PerkTooltipWidget = CreateWidget<UGRPerkTooltipWidget>(PC, PerkTooltipClass);
			if (PerkTooltipWidget)
			{
				PerkTooltipWidget->AddToViewport(100);
				PerkTooltipWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}

	if (ExitPerkButton)
	{
		ExitPerkButton->OnClicked.AddDynamic(this, &ThisClass::OnExitPerkClicked); 
	}
}

void UGRPerkHUDWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (ExitPerkButton)
	{
		ExitPerkButton->OnClicked.RemoveDynamic(this, &UGRPerkHUDWidget::OnExitPerkClicked);
	}

	if (PerkListContainer)
	{
		for (UWidget* Child : PerkListContainer->GetAllChildren())
		{
			if (UGRPerkListWidget* PerkListSlot = Cast<UGRPerkListWidget>(Child))
			{
				PerkListSlot->RemoveFromParent();
			}
		}
	}
}

void UGRPerkHUDWidget::ShowTooltipForSlot(UGRPerkSlotWidget* PerkSlot)
{
	ShowTooltip(PerkSlot);
}

void UGRPerkHUDWidget::ShowTooltip(UGRPerkSlotWidget* PerkSlot)
{
	if (!PerkTooltipWidget)
	{
		return;
	}

	UpdateTooltip(PerkSlot);

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}
	
	FVector2D MousePos;
	PC->GetMousePosition(MousePos.X, MousePos.Y);
	
	PerkTooltipWidget->SetPositionInViewport(MousePos + FVector2D(10, 10), true);
	PerkTooltipWidget->SetVisibility(ESlateVisibility::Visible);
}

void UGRPerkHUDWidget::HideTooltipForSlot(UGRPerkSlotWidget* PerkSlot)
{
	HideTooltip();
}

void UGRPerkHUDWidget::HideTooltip()
{
	if (!PerkTooltipWidget)
	{
		return;
	}

	PerkTooltipWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UGRPerkHUDWidget::UpdateHUDAndTooltip(UGRPerkSlotWidget* PerkSlot)
{
	UpdateGoodsText();

	if (PerkTooltipWidget && PerkTooltipWidget->IsVisible())
	{
		UpdateTooltip(PerkSlot);
	}
}

void UGRPerkHUDWidget::UpdateTooltip(UGRPerkSlotWidget* PerkSlot)
{
	if (!PerkTooltipWidget)
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

	FName PerkID = PerkSlot->GetPerkIDInSlot();
	
	FPerkInfoRow* Row = PerkTable->FindRow<FPerkInfoRow>(PerkID, TEXT(""));
	if (!Row)
	{
		return;
	}

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
	int32 Cost = (CurrentLevel + 1) * Row->CostPerLevel;
	
	PerkTooltipWidget->SetPerkTooltipHeader(Row->PerkName);
	PerkTooltipWidget->SetValuePerLevelText(Row->PerkDescription, Row->ValuePerLevel);
	PerkTooltipWidget->SetCurrentPerkLevelText(Row->PerkDescription, Row->ValuePerLevel, CurrentLevel);
	PerkTooltipWidget->SetNextPerkLevelText(Row->PerkDescription, Row->ValuePerLevel, CurrentLevel, Row->MaxLevel);
	PerkTooltipWidget->SetCostPerLevelText(CurrentLevel, Cost, Row->MaxLevel);
}

void UGRPerkHUDWidget::UpdateGoodsText()
{
	if (!GoodsText)
	{
		return;
	}

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

	GoodsText->SetGoodsTextInPerkHUD();
	GoodsText->SetGemCountText(PS->GetMetaGoods());
}

void UGRPerkHUDWidget::CreatePerkList()
{
	if (!PerkListClass || !PerkListContainer)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	for (int32 i = 0; i < PerkCategories.Num(); i++)
	{
		UGRPerkListWidget* NewPerkList = CreateWidget<UGRPerkListWidget>(PC, PerkListClass);
		if (!NewPerkList)
		{
			continue;
		}

		if (UVerticalBoxSlot* VerticalBoxSlot = PerkListContainer->AddChildToVerticalBox(NewPerkList))
		{
			VerticalBoxSlot->SetPadding(FMargin(0,0,0,50));
			VerticalBoxSlot->SetVerticalAlignment(VAlign_Bottom);
		}

		UE_LOG(LogTemp, Warning, TEXT("Creating PerkList %d : %s"), i, *PerkCategories[i].ToString());
		NewPerkList->SetupList(this);
		NewPerkList->SetPerkCategoryText(PerkCategories[i]);
		NewPerkList->CreateAllSlot(PerkCategories[i]);
	}
}

void UGRPerkHUDWidget::OnExitPerkClicked()
{
	AGRLobbyPlayerController* LobbyPlayerController = GetOwningPlayer<AGRLobbyPlayerController>();
	if (!IsValid(LobbyPlayerController))
	{
		return;
	}
	LobbyPlayerController->HidePerkWidget();
	LobbyPlayerController->ShowLobbyWidget();
}
