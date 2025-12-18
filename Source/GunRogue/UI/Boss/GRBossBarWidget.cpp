#include "UI/Boss/GRBossBarWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "AI/Character/GRLuwoAICharacter.h"

void UGRBossBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGRBossBarWidget::NativeDestruct()
{
	if (BossCharacter)
	{
		BossCharacter->OnBossHealthChanged.RemoveAll(this);
		BossCharacter->OnBossShieldChanged.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UGRBossBarWidget::SetBoss(AGRLuwoAICharacter* InBoss)
{
	BossCharacter = InBoss;
	if (!BossCharacter)
	{
		return;
	}
	BossName = FText::FromString(BossCharacter->GetName());
	BossNameText->SetText(BossName);

	SetMaxHealth(BossCharacter->GetBossMaxHealth());
	SetHealth(BossCharacter->GetBossHealth());

	SetMaxShield(BossCharacter->GetBossMaxShield());
	SetShield(BossCharacter->GetBossShield());

	BossCharacter->OnBossHealthChanged.AddUObject(this, &UGRBossBarWidget::SetHealth);
	BossCharacter->OnBossShieldChanged.AddUObject(this, &UGRBossBarWidget::SetShield);
}

void UGRBossBarWidget::SetHealthBar(float CurrentHP, float MaxHP)
{
	if (!BossHealthBar || !BossHealthBarText)
	{
		return;
	}

	float HPPercent = (MaxHP > 0.f) ? (CurrentHP / MaxHP) : 0.f;
	BossHealthBar->SetPercent(HPPercent);

	FText HealthText = FText::FromString(FString::Printf(TEXT("%.1f / %.1f"), CurrentHP, MaxHP));

	BossHealthBarText->SetText(HealthText);
}

void UGRBossBarWidget::SetHealth(float Value)
{
	Health = Value;
	SetHealthBar(Health, MaxHealth);
}

void UGRBossBarWidget::SetMaxHealth(float Value)
{
	MaxHealth = Value;
	SetHealthBar(Health, MaxHealth);
}

void UGRBossBarWidget::SetShieldBar(float CurrentSD, float MaxSD)
{
	if (!BossShieldBar || !BossShieldBarText)
	{
		return;
	}

	float ShieldPercent = (MaxSD > 0.f) ? (CurrentSD / MaxSD) : 0.f;
	BossShieldBar->SetPercent(ShieldPercent);

	FText ShieldText = FText::FromString(FString::Printf(TEXT("%.1f / %.1f"), CurrentSD, MaxSD));

	BossShieldBarText->SetText(ShieldText);
}

void UGRBossBarWidget::SetShield(float Value)
{
	Shield = Value;
	SetShieldBar(Shield, MaxShield);
}

void UGRBossBarWidget::SetMaxShield(float Value)
{
	MaxShield = Value;
	SetShieldBar(Shield, MaxShield);
}
