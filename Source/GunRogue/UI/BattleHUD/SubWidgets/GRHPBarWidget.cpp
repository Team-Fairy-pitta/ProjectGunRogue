// GRHPBarWidget.cpp


#include "GRHPBarWidget.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UGRHPBarWidget::SetShieldBar(float CurrentShield, float InMaxShield)
{
	if (!ShieldBar || !ShieldBarText)
	{
		return;
	}

	float ShieldPercent = (InMaxShield > 0.f) ? (CurrentShield / InMaxShield) : 0.f;
	ShieldBar->SetPercent(ShieldPercent);

	int32 CurrentShiedInt = FMath::RoundToInt(CurrentShield);
	int32 MaxShiedInt = FMath::RoundToInt(InMaxShield);
	FText ShieldText = FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentShiedInt, MaxShiedInt));
	
	ShieldBarText->SetText(ShieldText);
}

void UGRHPBarWidget::SetHPBar(float CurrentHP, float MaxHP)
{
	if (!HPBar || !HPBarText)
	{
		return;
	}

	float HPPercent = (MaxHP > 0.f) ? (CurrentHP / MaxHP) : 0.f;
	HPBar->SetPercent(HPPercent);

	int32 CurrentHPInt = FMath::RoundToInt(CurrentHP);
	int32 MaxHPInt = FMath::RoundToInt(MaxHP);
	FText HPText = FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentHPInt, MaxHPInt));

	HPBarText->SetText(HPText);
}

void UGRHPBarWidget::SetHealth(float Value)
{
	Health = Value;
	SetHPBar(Health, MaxHealth);
}

void UGRHPBarWidget::SetMaxHealth(float Value)
{
	MaxHealth = Value;
	SetHPBar(Health, MaxHealth);
}

void UGRHPBarWidget::SetShield(float Value)
{
	Shield = Value;
	SetShieldBar(Shield, MaxShield);
}

void UGRHPBarWidget::SetMaxShield(float Value)
{
	MaxShield = Value;
	SetShieldBar(Shield, MaxShield);
}
