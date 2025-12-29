#include "UI/Level1/GRLevel1SelectWidget.h"
#include "UI/Level1/GRLevel1RoomWidget.h"
#include "UI/Level1/GRLevel1PopupWidget.h"
#include "Components/UniformGridPanel.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "GameModes/Level1/GRLevel1Data.h"

void UGRLevel1SelectWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UGRLevel1SelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CreateRoomWidgets();
	SetWidgetFocusable();
}

FReply UGRLevel1SelectWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		APlayerController* PlayerController = GetOwningPlayer();
		AGRBattlePlayerController* BattlePlayerController = Cast<AGRBattlePlayerController>(PlayerController);

		if (IsValid(BattlePlayerController))
		{
			BattlePlayerController->HideLevel1SelectWidget();
		}

		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UGRLevel1SelectWidget::InitWidget(const FGRLevel1Data& Level1Data, AGRLevel1ControlPanel* ControlPanel)
{
	CachedControlPanel = ControlPanel;

	const TArray<FGRLevel1Node>& Nodes = Level1Data.GetNodes();

	int32 TotalRoomCount = FGRLevel1Data::RowCount * FGRLevel1Data::ColCount;

	if (RoomWidgetInstances.Num() != TotalRoomCount)
	{
		return;
	}

	for (int32 Row = 0; Row < FGRLevel1Data::RowCount; ++Row)
	{
		for (int32 Col = 0; Col < FGRLevel1Data::ColCount; ++Col)
		{
			int32 Index = Row * FGRLevel1Data::ColCount + Col;
			const FGRLevel1Node& Node = Nodes[Index];
			RoomWidgetInstances[Index]->InitRoomWidget(Index, Node, this);
		}
	}
}

void UGRLevel1SelectWidget::ResetWidget()
{
	CachedControlPanel = nullptr;
}

void UGRLevel1SelectWidget::OnRoomClicked(int32 Index)
{
	APlayerController* PlayerController = GetOwningPlayer();
	AGRBattlePlayerController* BattlePlayerController = Cast<AGRBattlePlayerController>(PlayerController);
	if (!IsValid(BattlePlayerController))
	{
		return;
	}

	BattlePlayerController->ServerRPC_OnSelectNextRoom(Index, CachedControlPanel);
}

void UGRLevel1SelectWidget::ShowPopup()
{
	if (PopupWidget)
	{
		PopupWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UGRLevel1SelectWidget::SetPopupText(const FGRLevel1Node& Level1Data, int32 Index)
{
	if (PopupWidget)
	{
		FString StatusString;
		FString TypeString;
		switch (Level1Data.NodeStatus)
		{
		case ENodeStatus::NONE:
			StatusString = FString(TEXT("비활성"));
			break;
		case ENodeStatus::CURRENT:
			StatusString = FString(TEXT("현재 위치"));
			break;
		case ENodeStatus::NEXT:
			StatusString = FString(TEXT("이동 가능"));
			break;
		case ENodeStatus::CLEARD:
			StatusString = FString(TEXT("완료"));
			break;
		}

		switch (Level1Data.NodeType)
		{
		case ENodeType::BASE:
			TypeString = FString(TEXT("시작 지점"));
			break;
		case ENodeType::NORMAL:
			TypeString = FString(TEXT("일반 구역"));
			break;
		case ENodeType::HARD:
			TypeString = FString(TEXT("적 개체 다수 감지"));
			break;
		case ENodeType::BOSS:
			TypeString = FString(TEXT("높은 에너지 반응"));
			break;
		}
		PopupWidget->SetPopupText(Index, StatusString, TypeString);
	}
}

void UGRLevel1SelectWidget::HidePopup()
{
	if (PopupWidget)
	{
		PopupWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGRLevel1SelectWidget::CreateRoomWidgets()
{
	int32 TotalRoomCount = FGRLevel1Data::RowCount * FGRLevel1Data::ColCount;

	if (RoomWidgetInstances.Num() == TotalRoomCount)
	{
		return;
	}

	if (RoomWidgetClass && RoomGridPanel)
	{
		APlayerController* PC = GetOwningPlayer();
		if (IsValid(PC))
		{
			for (int32 Row = 0; Row < FGRLevel1Data::RowCount; ++Row)
			{
				for (int32 Col = 0; Col < FGRLevel1Data::ColCount; ++Col)
				{
					UGRLevel1RoomWidget* Room = CreateWidget<UGRLevel1RoomWidget>(PC, RoomWidgetClass);
					if (Room)
					{
						RoomGridPanel->AddChildToUniformGrid(Room, Row, Col);
						RoomWidgetInstances.Add(Room);
					}
				}
			}
		}
	}
}

void UGRLevel1SelectWidget::SetWidgetFocusable()
{
	bIsFocusable = true;
}
