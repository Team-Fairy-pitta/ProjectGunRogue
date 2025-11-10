// GRSkillSlotWidget.cpp


#include "GRSkillSlotWidget.h"

#include "Components/ProgressBar.h"
#include "UI/Common/GRBaseTextWidget.h"

void UGRSkillSlotWidget::SetSkillCountText(int32 InCount)
{
	if (!SkillCountText)
	{
		return;
	}

	CurrentSkillCount = InCount;
	
	if (InCount > 0)
	{
		SkillCountText->SetText(FText::AsNumber(CurrentSkillCount));

		SkillCountText->SetVisibility(ESlateVisibility::Visible);
		
	}
	else
	{
		SkillCountText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGRSkillSlotWidget::SetCooldown(float RemainingTime, float MaxTime)
{
	if (!SkillCooldown || !SkillCooldownText)
	{
		return;
	}

	if (MaxTime <= 0)
	{
		return;
	}

	float CooldownPercent = FMath::Clamp(RemainingTime / MaxTime, 0.0f, 1.0f);
	SkillCooldown->SetPercent(CooldownPercent);

	if (RemainingTime <= 0.0f)
	{
		SkillCooldownText->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		SkillCooldownText->SetVisibility(ESlateVisibility::Visible);
		SkillCooldownText->SetText(FText::AsNumber(FMath::RoundToInt(RemainingTime)));	
	}
}

void UGRSkillSlotWidget::SetCharge(float CurrentCharge, float MaxCharge)
{
	if (!SkillCooldown || !SkillCooldownText)
	{
		return;
	}

	if (MaxCharge <= 0)
	{
		return;
	}

	float ChargePercent = FMath::Clamp(CurrentCharge / MaxCharge, 0.0f, 1.0f);
	SkillCooldown->SetPercent(ChargePercent);

	SkillCooldownText->SetText(FText::AsNumber(FMath::RoundToInt(CurrentCharge)));
}

void UGRSkillSlotWidget::SetSkillKey(const FText& InText)
{
	if (!SkillKeyText)
	{
		return;
	}

	SkillKeyText->SetText(InText);
}

void UGRSkillSlotWidget::StartCooldown(float MaxTime)
{
	if (!GetWorld())
	{
		return;
	}

	CooldownRemainingTime = MaxTime;
	CooldownMaxTime = MaxTime;

	SetCooldown(CooldownRemainingTime, CooldownMaxTime);

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
	SetCooldown(CooldownRemainingTime, CooldownMaxTime);

	if (CooldownRemainingTime <= 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
		SetCooldown(0.0f, CooldownMaxTime);
	}
}
