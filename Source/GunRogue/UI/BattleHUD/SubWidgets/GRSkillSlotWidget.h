// GRSkillSlotWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRSkillSlotWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UImage;

UCLASS()
class GUNROGUE_API UGRSkillSlotWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* SkillCooldown;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillCooldownText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillKeyText;

	UPROPERTY(meta = (BindWidget))
	UImage* SkillIcon;

public:
	void SetCooldown(float RemainingTime, float MaxTime);
	void SetSkillIcon(UTexture2D* InIcon);
	void SetSkillKey(const FText& InText);
};
