// GRTeamStatusWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRTeamStatusWidget.generated.h"

class UGRHPBarWidget;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRTeamStatusWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	UGRHPBarWidget* TeamHPBar;

	virtual void NativeConstruct() override;

public:
	void SetTeamShieldBar(float CurrentShield, float MaxShield);
	
	void SetTeamHPBar(float CurrentHP, float MaxHP);
};
