// GRPlayerStatusWidget.cpp


#include "GRPlayerStatusWidget.h"

#include "GRBuffIconWidget.h"
#include "GRHPBarWidget.h"
#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"
#include "AbilitySystem/GRGameplayEffect.h"

void UGRPlayerStatusWidget::SetPlayerShieldBar(float CurrentShield, float MaxShield)
{
	SetPlayerShield(CurrentShield);
	SetPlayerMaxShield(MaxShield);
}

void UGRPlayerStatusWidget::SetPlayerHPBar(float CurrentHP, float MaxHP)
{
	SetPlayerHealth(CurrentHP);
	SetPlayerMaxHealth(MaxHP);
}

void UGRPlayerStatusWidget::SetPlayerHealth(float Value)
{
	if (!PlayerHPBar)
	{
		return;
	}
	PlayerHPBar->SetHealth(Value);
}

void UGRPlayerStatusWidget::SetPlayerMaxHealth(float Value)
{
	if (!PlayerHPBar)
	{
		return;
	}
	PlayerHPBar->SetMaxHealth(Value);
}

void UGRPlayerStatusWidget::SetPlayerShield(float Value)
{
	if (!PlayerHPBar)
	{
		return;
	}
	PlayerHPBar->SetShield(Value);
}

void UGRPlayerStatusWidget::SetPlayerMaxShield(float Value)
{
	if (!PlayerHPBar)
	{
		return;
	}
	PlayerHPBar->SetMaxShield(Value);
}

void UGRPlayerStatusWidget::CreateBuffIcon()
{
	if (!BuffIconClass || !BuffIconContainer)
	{
		return;
	}
	
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}
	
	UGRBuffIconWidget* NewBuffIcon = CreateWidget<UGRBuffIconWidget>(PC, BuffIconClass);
	if (!NewBuffIcon)
	{
		return;
	}

	if (UWrapBoxSlot* WrapBoxSlot = BuffIconContainer->AddChildToWrapBox(NewBuffIcon))
	{
		WrapBoxSlot->SetPadding(FMargin(2.0f));
		WrapBoxSlot->SetHorizontalAlignment(HAlign_Left);
		WrapBoxSlot->SetVerticalAlignment(VAlign_Center);
	}
	
	BuffIcons.Add(NewBuffIcon);
}

void UGRPlayerStatusWidget::AddBuffIcon(TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UGRGameplayEffect* EffectCDO = GameplayEffectClass->GetDefaultObject<UGRGameplayEffect>();
	if (!EffectCDO)
	{
		return;
	}

	if (!EffectCDO->bShowBuffIconInUI)
	{
		return;
	}

	if (BuffEffects.Find(GameplayEffectClass) != INDEX_NONE)
	{
		return;
	}
	BuffEffects.Add(GameplayEffectClass);

	int32 BuffIndex = BuffIcons.Num();
	CreateBuffIcon();
	if (!BuffIcons.IsValidIndex(BuffIndex))
	{
		return;
	}

	UGRBuffIconWidget* NewBuffIcon = BuffIcons[BuffIndex];
	if (!NewBuffIcon)
	{
		return;
	}

	NewBuffIcon->SetBuffIcon(EffectCDO->EffectIcon);

}

void UGRPlayerStatusWidget::RemoveBuffIcon(TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	int32 BuffIndex = BuffEffects.Find(GameplayEffectClass);
	if (BuffIndex == INDEX_NONE)
	{
		return;
	}

	if (!BuffIcons.IsValidIndex(BuffIndex))
	{
		return;
	}

	if (!BuffIconContainer)
	{
		return;
	}

	BuffIcons.RemoveAt(BuffIndex);
	BuffEffects.RemoveAt(BuffIndex);
	BuffIconContainer->RemoveChildAt(BuffIndex);
}
