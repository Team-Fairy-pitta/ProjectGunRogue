#include "GRSkillListWidget.h"
#include "GRSkillSlotWidget.h"
#include "Character/GRPawnData.h"
#include "Character/GRCharacter.h"
#include "Player/GRPlayerController.h"
#include "AbilitySystemComponent.h"

void UGRSkillListWidget::NativeConstruct()
{
	CacheAbilitySystemComponent();
	SetSkillKeyText();
	SetSkillIcon();
	RegisterTimer();
}

void UGRSkillListWidget::NativeDestruct()
{
	UnregisterTimer();
}

void UGRSkillListWidget::SetSkillKeyText()
{
	// [TODO] player controller에서 키 설정을 가져와서, 실제 입력 키 값을 넘겨야 함
	if (FirstSkillSlot)
	{
		FText FirstSkillKey = FText::FromString(FString(TEXT("Q")));
		FirstSkillSlot->SetSkillKey(FirstSkillKey);
	}

	if (SecondSkillSlot)
	{
		FText SecondSkillKey = FText::FromString(FString(TEXT("E")));
		SecondSkillSlot->SetSkillKey(SecondSkillKey);
	}
}

void UGRSkillListWidget::SetSkillIcon()
{
	if (!GetWorld())
	{
		return;
	}

	const UGRPawnData* PawnData = GetPawnData();
	if (!PawnData)
	{
		UE_LOG(LogTemp, Error, TEXT("PawnData is INVALID"));
		return;
	}

	CachedSkillInfo_Q = PawnData->SkillInfo_Q;
	CachedSkillInfo_E = PawnData->SkillInfo_E;

	if (FirstSkillSlot)
	{
		FirstSkillSlot->SetSkillIcon(CachedSkillInfo_Q.SkillIcon);
	}

	if (SecondSkillSlot)
	{
		SecondSkillSlot->SetSkillIcon(CachedSkillInfo_E.SkillIcon);
	}
}

void UGRSkillListWidget::RegisterTimer()
{
	if (!GetWorld())
	{
		return;
	}

	if (CoolDownUpdateTimer.IsValid())
	{
		UnregisterTimer();
	}

	GetWorld()->GetTimerManager().SetTimer(CoolDownUpdateTimer, this, &ThisClass::UpdateSkillCoolDown, UpdateSkillCoolDownInterval, true);
}

void UGRSkillListWidget::UnregisterTimer()
{
	if (!GetWorld())
	{
		return;
	}

	if (CoolDownUpdateTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(CoolDownUpdateTimer);
		CoolDownUpdateTimer.Invalidate();
	}
}

void UGRSkillListWidget::UpdateSkillCoolDown()
{
	if (FirstSkillSlot)
	{
		float RemainTime_Q;
		float MaxTime_Q;
		GetCooldownValue(CachedSkillInfo_Q.SkillCooldownTag, RemainTime_Q, MaxTime_Q);
		FirstSkillSlot->SetCooldown(RemainTime_Q, MaxTime_Q);
	}

	if (SecondSkillSlot)
	{
		float RemainTime_E;
		float MaxTime_E;
		GetCooldownValue(CachedSkillInfo_E.SkillCooldownTag, RemainTime_E, MaxTime_E);
		SecondSkillSlot->SetCooldown(RemainTime_E, MaxTime_E);
	}
}

const UGRPawnData* UGRSkillListWidget::GetPawnData() const
{
	AGRPlayerController* GRPlayerController = GetOwningPlayer<AGRPlayerController>();
	if (!IsValid(GRPlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerController is INVALID"));
		return nullptr;
	}

	AGRCharacter* GRCharacter = GRPlayerController->GetPawn<AGRCharacter>();
	if (!IsValid(GRCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("GRCharacter is INVALID"));
		return nullptr;
	}

	return GRCharacter->GetPawnData();
}

void UGRSkillListWidget::CacheAbilitySystemComponent()
{
	AGRPlayerController* GRPlayerController = GetOwningPlayer<AGRPlayerController>();
	if (!IsValid(GRPlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerController is INVALID"));
		return;
	}

	AGRCharacter* GRCharacter = GRPlayerController->GetPawn<AGRCharacter>();
	if (!IsValid(GRCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("GRCharacter is INVALID"));
		return;
	}

	CachedASC = GRCharacter->GetAbilitySystemComponent();
}

void UGRSkillListWidget::GetCooldownValue(FGameplayTag TargetTag, OUT float& RemainTime, OUT float& MaxTime)
{
	FGameplayEffectQuery Query;
	Query.OwningTagQuery = FGameplayTagQuery::MakeQuery_MatchAnyTags(FGameplayTagContainer(TargetTag));
	TArray<float> RemainResults = CachedASC->GetActiveEffectsTimeRemaining(Query);
	TArray<float> DurationResults = CachedASC->GetActiveEffectsDuration(Query);

	if (RemainResults.Num() > 0)
	{
		RemainTime = RemainResults[0];
	}
	else
	{
		RemainTime = 0.0f;
	}

	if (DurationResults.Num() > 0)
	{
		MaxTime = DurationResults[0];
	}
	else
	{
		MaxTime = 1.0f;
	}
}
