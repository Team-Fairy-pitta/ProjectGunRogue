// GRTeamStatusListWidget.cpp


#include "GRTeamStatusListWidget.h"

#include "GRTeamStatusWidget.h"

void UGRTeamStatusListWidget::SetTeamShieldBar(float CurrentShield, float MaxShield)
{
	if (!TeamStatus) return;

	TeamStatus->SetTeamShieldBar(CurrentShield, MaxShield);
}

void UGRTeamStatusListWidget::SetTeamHPBar(float CurrentHP, float MaxHP)
{
	if (!TeamStatus) return;
	
	TeamStatus->SetTeamHPBar(CurrentHP, MaxHP);
}
