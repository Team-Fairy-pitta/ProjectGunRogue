// GRSkillSlotWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRSkillSlotWidget.generated.h"

class UGRBaseTextWidget;
class UProgressBar;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRSkillSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	int32 GetSkillCount() const { return CurrentSkillCount; }

protected:
	UPROPERTY(meta = (BindWidget))
	UGRBaseTextWidget* SkillCountText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* SkillCooldown;

	UPROPERTY(meta = (BindWidget))
	UGRBaseTextWidget* SkillCooldownText;

	UPROPERTY(meta = (BindWidget))
	UGRBaseTextWidget* SkillKeyText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Skill")
	int32 CurrentSkillCount = 0;

public:
	void SetSkillCountText(int32 InCount);

	void SetCooldown(float RemainingTime, float MaxTime);

	void SetCharge(float CurrentCharge, float MaxCharge);

	void SetSkillKey(const FText& InText);

	void StartCooldown(float MaxTime);

private:
	FTimerHandle CooldownTimerHandle;

	float CooldownRemainingTime = 0.0f;
	
	float CooldownMaxTime = 0.0f;

	UFUNCTION()
	void UpdateCooldown();
};
