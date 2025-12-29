#include "UI/Level1/GRLevel1RoomWidget.h"
#include "UI/Level1/GRLevel1SelectWidget.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UGRLevel1RoomWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	ParentWidget->SetPopupText(CachedNodeInfo, Index);
	ParentWidget->ShowPopup();
}

void UGRLevel1RoomWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	ParentWidget->HidePopup();
}

void UGRLevel1RoomWidget::InitRoomWidget(int32 InIndex, const FGRLevel1Node& Level1Data, UGRLevel1SelectWidget* InParentWidget)
{
	if (!Border)
	{
		return;
	}
	if (!TypeIcon)
	{
		return;
	}
	if (!RoomButton)
	{
		return;
	}

	Index = InIndex;
	ParentWidget = InParentWidget;
	CachedNodeInfo = Level1Data;

	if (!RoomButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnButtonClicked))
	{
		RoomButton->OnClicked.AddDynamic(this, &ThisClass::OnButtonClicked);
	}

	switch (CachedNodeInfo.NodeStatus)
	{
	case ENodeStatus::NONE:
		Border->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f));
		RoomButton->SetIsEnabled(false);
		break;
	case ENodeStatus::CURRENT:
		Border->SetBrushColor(FLinearColor(0.0f, 1.0f, 0.0f));
		RoomButton->SetIsEnabled(false);
		break;
	case ENodeStatus::NEXT:
		Border->SetBrushColor(FLinearColor(0.0f, 0.0f, 1.0f));
		RoomButton->SetIsEnabled(true);
		break;
	case ENodeStatus::CLEARD:
		Border->SetBrushColor(FLinearColor(0.2f, 0.2f, 0.2f));
		RoomButton->SetIsEnabled(false);
		break;
	}

	switch (CachedNodeInfo.NodeType)
	{
	case ENodeType::BASE:
		TypeIcon->SetVisibility(ESlateVisibility::Visible);
		TypeIcon->SetBrushTintColor(BaseIconColor);
		TypeIcon->SetBrushFromTexture(BaseIcon);
		break;
	case ENodeType::NORMAL:
		TypeIcon->SetVisibility(ESlateVisibility::Hidden);
		break;
	case ENodeType::HARD:
		TypeIcon->SetVisibility(ESlateVisibility::Visible);
		TypeIcon->SetBrushTintColor(HardIconColor);
		TypeIcon->SetBrushFromTexture(HardIcon);
		break;
	case ENodeType::BOSS:
		TypeIcon->SetVisibility(ESlateVisibility::Visible);
		TypeIcon->SetBrushTintColor(BossIconColor);
		TypeIcon->SetBrushFromTexture(BossIcon);
		break;
	}
}

void UGRLevel1RoomWidget::OnButtonClicked()
{
	if (ParentWidget && Index != -1)
	{
		ParentWidget->OnRoomClicked(Index);
	}
}
