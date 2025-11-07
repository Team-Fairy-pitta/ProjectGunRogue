// GRTeamStatusListWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRTeamStatusListWidget.generated.h"

class UGRTeamStatusWidget;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRTeamStatusListWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UGRTeamStatusWidget* GetTeamStatusWidget() const { return TeamStatus; }
	
protected:
	UPROPERTY(meta = (BindWidget))
	UGRTeamStatusWidget* TeamStatus;

public:
	void SetTeamShieldBar(float CurrentShield, float MaxShield);

	void SetTeamHPBar(float CurrentHP, float MaxHP);
};
