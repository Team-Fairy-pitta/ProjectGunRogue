#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/GRPawnData.h"
#include "GRSkillListWidget.generated.h"

class UGRSkillSlotWidget;
class UAbilitySystemComponent;

UCLASS()
class GUNROGUE_API UGRSkillListWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void NativeConstruct() override;
	void NativeDestruct() override;

	UGRSkillSlotWidget* GetFirstSkillSlot() const { return FirstSkillSlot; }
	UGRSkillSlotWidget* GetSecondSkillSlot() const { return SecondSkillSlot; }

protected:
	UPROPERTY(meta = (BindWidget))
	UGRSkillSlotWidget* FirstSkillSlot;

	UPROPERTY(meta = (BindWidget))
	UGRSkillSlotWidget* SecondSkillSlot;

	FTimerHandle CoolDownUpdateTimer;

private:
	void SetSkillKeyText();
	void SetSkillIcon();
	void RegisterTimer();
	void UnregisterTimer();
	void UpdateSkillCoolDown();

	const UGRPawnData* GetPawnData() const;
	void CacheAbilitySystemComponent();
	void GetCooldownValue(FGameplayTag TargetTag, OUT float& RemainTime, OUT float& MaxTime);

	const float UpdateSkillCoolDownInterval = 0.1f;

	FGRCharacterSkillInfo CachedSkillInfo_Q;
	FGRCharacterSkillInfo CachedSkillInfo_E;
	UAbilitySystemComponent* CachedASC;
};
