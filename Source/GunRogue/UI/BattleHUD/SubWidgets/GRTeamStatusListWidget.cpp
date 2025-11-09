// GRTeamStatusListWidget.cpp


#include "GRTeamStatusListWidget.h"
#include "GRTeamStatusWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UGRTeamStatusListWidget::SetTeamShieldBar(int32 Index, float CurrentShield, float MaxShield)
{
	if (!TeamStatusArray.IsValidIndex(Index)) return;

	if (UGRTeamStatusWidget* TeamStatus = TeamStatusArray[Index])
	{
		TeamStatus->SetTeamShieldBar(CurrentShield, MaxShield);
	}
}

void UGRTeamStatusListWidget::SetTeamHPBar(int32 Index, float CurrentHP, float MaxHP)
{
	if (!TeamStatusArray.IsValidIndex(Index)) return;

	if (UGRTeamStatusWidget* TeamStatus = TeamStatusArray[Index])
	{
		TeamStatus->SetTeamHPBar(CurrentHP, MaxHP);
	}
}

void UGRTeamStatusListWidget::CreateTeamStatus()
{
	if (!TeamStatusClass || !TeamStatusContainer) return;

	UWorld* World = GetWorld();
	if (!World) return;
	
	UGRTeamStatusWidget* NewTeamStatus = CreateWidget<UGRTeamStatusWidget>(World, TeamStatusClass);
	if (!NewTeamStatus) return;

	FWidgetTransform WidgetTransform;
	WidgetTransform.Angle = 180.0f;
	NewTeamStatus->SetRenderTransform(WidgetTransform);

	if (UVerticalBoxSlot* VerticalBoxSlot = TeamStatusContainer->AddChildToVerticalBox(NewTeamStatus))
	{
		VerticalBoxSlot->SetPadding(FMargin(0,0,0,2.0f));
		VerticalBoxSlot->SetHorizontalAlignment(HAlign_Center);
	}

	TeamStatusArray.Add(NewTeamStatus);
}
