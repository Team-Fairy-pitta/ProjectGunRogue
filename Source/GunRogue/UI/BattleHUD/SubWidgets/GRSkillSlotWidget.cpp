// GRSkillSlotWidget.cpp


#include "GRSkillSlotWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

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

void UGRSkillSlotWidget::SetSkillKey(const FText& InText)
{
	if (!SkillKeyText)
	{
		return;
	}

	SkillKeyText->SetText(InText);
}

void UGRSkillSlotWidget::SetSkillIcon(UTexture2D* InIcon)
{
	if (!SkillIcon)
	{
		return;
	}
	if (!InIcon)
	{
		return;
	}

	SkillIcon->SetBrushFromTexture(InIcon);
}
