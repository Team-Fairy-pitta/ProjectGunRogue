#include "UI/Level1/GRLevel1RoomWidget.h"
#include "GameModes/Level1/GRLevel1Data.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

void UGRLevel1RoomWidget::InitRoomWidget(const FGRLevel1Node& Level1Data)
{
	if (!Border)
	{
		return;
	}
	if (!MapIndex)
	{
		return;
	}

	switch (Level1Data.NodeStatus)
	{
	case ENodeStatus::NONE:
		MapIndex->SetText(FText());
		Border->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f));
		break;
	case ENodeStatus::CURRENT:
		MapIndex->SetText(FText());
		Border->SetBrushColor(FLinearColor(0.0f, 1.0f, 0.0f));
		break;
	case ENodeStatus::NEXT:
		MapIndex->SetText(FText());
		Border->SetBrushColor(FLinearColor(0.0f, 0.0f, 1.0f));
		break;
	case ENodeStatus::CLEARD:
		MapIndex->SetText(FText());
		Border->SetBrushColor(FLinearColor(0.2f, 0.2f, 0.2f));
		break;
	}
}
