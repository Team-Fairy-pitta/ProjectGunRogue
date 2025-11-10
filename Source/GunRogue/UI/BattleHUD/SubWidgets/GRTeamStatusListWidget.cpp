// GRTeamStatusListWidget.cpp


#include "GRTeamStatusListWidget.h"
#include "GRTeamStatusWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UGRTeamStatusListWidget::SetTeamShieldBar(int32 Index, float CurrentShield, float MaxShield)
{
	if (!TeamStatusArray.IsValidIndex(Index))
	{
		return;
	}

	if (UGRTeamStatusWidget* TeamStatus = TeamStatusArray[Index])
	{
		TeamStatus->SetTeamShieldBar(CurrentShield, MaxShield);
	}
}

void UGRTeamStatusListWidget::SetTeamHPBar(int32 Index, float CurrentHP, float MaxHP)
{
	if (!TeamStatusArray.IsValidIndex(Index))
	{
		return;
	}

	if (UGRTeamStatusWidget* TeamStatus = TeamStatusArray[Index])
	{
		TeamStatus->SetTeamHPBar(CurrentHP, MaxHP);
	}
}

void UGRTeamStatusListWidget::CreateTeamStatus()
{
	if (!TeamStatusClass || !TeamStatusContainer)
	{
		return;
	}
	
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}
	
	UGRTeamStatusWidget* NewTeamStatus = CreateWidget<UGRTeamStatusWidget>(PC, TeamStatusClass);
	if (!NewTeamStatus)
	{
		return;
	}
	
	if (UVerticalBoxSlot* VerticalBoxSlot = TeamStatusContainer->AddChildToVerticalBox(NewTeamStatus))
	{
		VerticalBoxSlot->SetPadding(FMargin(0,0,0,2.0f));
		VerticalBoxSlot->SetHorizontalAlignment(HAlign_Center);
	}

	TeamStatusArray.Add(NewTeamStatus);
}
