// GRSkillSlotWidget.cpp


#include "UI/GRSkillSlotWidget.h"

#include "Components/ProgressBar.h"
#include "UI/GRBaseTextWidget.h"

void UGRSkillSlotWidget::SetSkillCountText(int32 InCount, const FLinearColor& InColor)
{
	if (!SkillCountText) return;

	CurrentSkillCount = InCount;
	
	if (InCount > 0)
	{
		SkillCountText->SetTextAndColor(
			FText::AsNumber(CurrentSkillCount),
			FSlateColor(InColor)
			);

		SkillCountText->SetVisibility(ESlateVisibility::Visible);
		
	}
	else
	{
		SkillCountText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGRSkillSlotWidget::SetCooldown(float RemainingTime, float MaxTime, const FLinearColor& InColor)
{
	if (!SkillCooldown || !SkillCooldownText) return;

	float CooldownPercent = FMath::Clamp(RemainingTime / MaxTime, 0.0f, 1.0f);
	SkillCooldown->SetPercent(CooldownPercent);

	if (RemainingTime <= 0.0f)
	{
		SkillCooldownText->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		SkillCooldownText->SetVisibility(ESlateVisibility::Visible);
		SkillCooldownText->SetTextAndColor(
			FText::AsNumber(FMath::RoundToInt(RemainingTime)),
			FSlateColor(InColor)
		);	
	}
}

void UGRSkillSlotWidget::SetCharge(float CurrentCharge, float MaxCharge, const FLinearColor& InColor)
{
	if (!SkillCooldown || !SkillCooldownText) return;

	float ChargePercent = FMath::Clamp(CurrentCharge / MaxCharge, 0.0f, 1.0f);
	SkillCooldown->SetPercent(ChargePercent);

	SkillCooldownText->SetTextAndColor(
		FText::AsNumber(FMath::RoundToInt(CurrentCharge)),
		FSlateColor(InColor)
	);
}

void UGRSkillSlotWidget::SetSkillKey(const FText& InText, const FLinearColor& InColor)
{
	if (!SkillKeyText) return;

	SkillKeyText->SetTextAndColor(
		InText,
		FSlateColor(InColor)
	);
}

void UGRSkillSlotWidget::StartCooldown(float MaxTime, const FLinearColor& InColor)
{
	if (!GetWorld()) return;

	CooldownRemainingTime = MaxTime;
	CooldownMaxTime = MaxTime;
	CooldownColor = InColor;

	SetCooldown(CooldownRemainingTime, CooldownMaxTime, InColor);

	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(
		CooldownTimerHandle,
		this,
		&UGRSkillSlotWidget::UpdateCooldown,
		1.0f,
		true
	);
}

void UGRSkillSlotWidget::UpdateCooldown()
{
	CooldownRemainingTime -= 1.0f;
	SetCooldown(CooldownRemainingTime, CooldownMaxTime, CooldownColor);

	if (CooldownRemainingTime <= 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
		SetCooldown(0.0f, CooldownMaxTime);
	}
}
