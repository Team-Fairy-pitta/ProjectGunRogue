#include "UI/Level1/GRLevel1RoomWidget.h"
#include "UI/Level1/GRLevel1SelectWidget.h"
#include "GameModes/Level1/GRLevel1Data.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Button.h"

void UGRLevel1RoomWidget::InitRoomWidget(int32 InIndex, const FGRLevel1Node& Level1Data, UGRLevel1SelectWidget* InParentWidget)
{
	if (!Border)
	{
		return;
	}
	if (!MapIndex)
	{
		return;
	}
	if (!RoomButton)
	{
		return;
	}

	Index = InIndex;
	ParentWidget = InParentWidget;

	if (!RoomButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnButtonClicked))
	{
		RoomButton->OnClicked.AddDynamic(this, &ThisClass::OnButtonClicked);
	}

	switch (Level1Data.NodeStatus)
	{
	case ENodeStatus::NONE:
		MapIndex->SetText(FText());
		Border->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f));
		RoomButton->SetIsEnabled(false);
		break;
	case ENodeStatus::CURRENT:
		MapIndex->SetText(FText());
		Border->SetBrushColor(FLinearColor(0.0f, 1.0f, 0.0f));
		RoomButton->SetIsEnabled(false);
		break;
	case ENodeStatus::NEXT:
		MapIndex->SetText(FText());
		Border->SetBrushColor(FLinearColor(0.0f, 0.0f, 1.0f));
		RoomButton->SetIsEnabled(true);
		break;
	case ENodeStatus::CLEARD:
		MapIndex->SetText(FText());
		Border->SetBrushColor(FLinearColor(0.2f, 0.2f, 0.2f));
		RoomButton->SetIsEnabled(false);
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
