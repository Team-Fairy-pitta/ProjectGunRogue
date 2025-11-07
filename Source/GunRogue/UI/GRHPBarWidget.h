// GRHPBarWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRHPBarWidget.generated.h"

class UGRBaseTextWidget;
class UProgressBar;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRHPBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UGRBaseTextWidget* GetShieldBarTextWidget() const { return ShieldBarText;}

	UGRBaseTextWidget* GetHPBarTextWidget() const { return HPBarText;}
	
protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ShieldBar;
	
	UPROPERTY(meta = (BindWidget))
	UGRBaseTextWidget* ShieldBarText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	UGRBaseTextWidget* HPBarText;

public:
	void SetShieldBar(float CurrentShield, float MaxShield, const FLinearColor& InColor = FLinearColor::White);
	
	void SetHPBar(float CurrentHP, float MaxHP, const FLinearColor& InColor = FLinearColor::White);
};
