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

	BossCharacter->OnBossHealthChanged.AddUObject(this, &UGRBossBarWidget::SetHealth);
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
