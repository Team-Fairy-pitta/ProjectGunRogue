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
	void SetSkillCountText(int32 InCount, const FLinearColor& InColor = FLinearColor::White);

	void SetCooldown(float RemainingTime, float MaxTime, const FLinearColor& InColor = FLinearColor::White);

	void SetCharge(float CurrentCharge, float MaxCharge, const FLinearColor& InColor = FLinearColor::White);

	void SetSkillKey(const FText& InText, const FLinearColor& InColor = FLinearColor::White);

	void StartCooldown(float MaxTime, const FLinearColor& InColor = FLinearColor::White);

private:
	FTimerHandle CooldownTimerHandle;

	float CooldownRemainingTime = 0.0f;
	
	float CooldownMaxTime = 0.0f;

	FLinearColor CooldownColor;

	UFUNCTION()
	void UpdateCooldown();
};
