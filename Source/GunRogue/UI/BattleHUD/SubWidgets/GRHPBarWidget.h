// GRHPBarWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRHPBarWidget.generated.h"

class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRHPBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UTextBlock* GetShieldBarTextWidget() const { return ShieldBarText;}

	UTextBlock* GetHPBarTextWidget() const { return HPBarText;}
	
protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ShieldBar;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ShieldBarText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HPBarText;

public:
	void SetShieldBar(float CurrentShield, float InMaxShield);
	void SetHPBar(float CurrentHP, float MaxHP);

	void SetHealth(float Value);
	void SetMaxHealth(float Value);
	void SetShield(float Value);
	void SetMaxShield(float Value);

private:
	float Health;
	float MaxHealth;
	float Shield;
	float MaxShield;
};
