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

void UGRTeamStatusListWidget::SetTeamCharacterThumbnail(int32 Index, UTexture2D* Thumbnail)
{
	if (!Thumbnail)
	{
		return;
	}

	if (!TeamStatusArray.IsValidIndex(Index))
	{
		return;
	}

	if (UGRTeamStatusWidget* TeamStatus = TeamStatusArray[Index])
	{
		TeamStatus->SetTeamCharacterThumbnail(Thumbnail);
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

void UGRTeamStatusListWidget::DestroyLastTeamStatus()
{
	if (GetTeamStatusWidgetCount() <= 0)
	{
		return;
	}
	if (!TeamStatusContainer)
	{
		return;
	}

	int32 LastIndex = TeamStatusArray.Num() - 1;
	TeamStatusArray.RemoveAt(LastIndex);

	int32 LastContainerIndex = TeamStatusContainer->GetChildrenCount() - 1;
	TeamStatusContainer->RemoveChildAt(LastContainerIndex);
}

int32 UGRTeamStatusListWidget::GetTeamStatusWidgetCount() const
{
	return TeamStatusArray.Num();
}
