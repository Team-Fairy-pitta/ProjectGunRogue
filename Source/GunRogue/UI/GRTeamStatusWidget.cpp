// GRTeamStatusWidget.cpp


#include "UI/GRTeamStatusWidget.h"

#include "GRBaseTextWidget.h"
#include "GRHPBarWidget.h"


void UGRTeamStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (TeamHPBar)
	{
		TeamHPBar->GetShieldBarTextWidget()->SetVisibility(ESlateVisibility::Collapsed);
		TeamHPBar->GetHPBarTextWidget()->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UGRTeamStatusWidget::SetTeamShieldBar(float CurrentShield, float MaxShield)
{
	if (!TeamHPBar) return;

	TeamHPBar->SetShieldBar(CurrentShield, MaxShield);
}

void UGRTeamStatusWidget::SetTeamHPBar(float CurrentHP, float MaxHP)
{
	if (!TeamHPBar) return;
	
	TeamHPBar->SetHPBar(CurrentHP, MaxHP);
}
