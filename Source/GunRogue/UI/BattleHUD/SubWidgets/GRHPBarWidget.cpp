// GRHPBarWidget.cpp


#include "GRHPBarWidget.h"

#include "UI/Common/GRBaseTextWidget.h"
#include "Components/ProgressBar.h"

void UGRHPBarWidget::SetShieldBar(float CurrentShield, float MaxShield)
{
	if (!ShieldBar || !ShieldBarText) return;

	float ShieldPercent = (MaxShield > 0.f) ? (CurrentShield / MaxShield) : 0.f;
	ShieldBar->SetPercent(ShieldPercent);

	FText ShieldText = FText::FromString(FString::Printf(TEXT("%d / %d"),
		FMath::RoundToInt(CurrentShield), FMath::RoundToInt(MaxShield)));
	
	ShieldBarText->SetText(ShieldText);
}

void UGRHPBarWidget::SetHPBar(float CurrentHP, float MaxHP)
{
	if (!HPBar || !HPBarText) return;

	float HPPercent = (MaxHP > 0.f) ? (CurrentHP / MaxHP) : 0.f;
	HPBar->SetPercent(HPPercent);

	FText HPText = FText::FromString(FString::Printf(TEXT("%d / %d"),
		FMath::RoundToInt(CurrentHP), FMath::RoundToInt(MaxHP)));

	HPBarText->SetText(HPText);
}
