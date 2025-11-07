// GRPlayerStatusWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRPlayerStatusWidget.generated.h"

class UGRHPBarWidget;
class UGRBuffIconWidget;
class UWrapBox;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRPlayerStatusWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UGRHPBarWidget* PlayerHPBar;

	UPROPERTY(meta = (BindWidget))
	UWrapBox* BuffIconContainer;

	UPROPERTY(EditDefaultsOnly, Category="Buff")
	TSubclassOf<UGRBuffIconWidget> BuffIconClass;

	UPROPERTY()
	TArray<UGRBuffIconWidget*> BuffIcons;

public:
	void SetPlayerShieldBar(float CurrentShield, float MaxShield);

	void SetPlayerHPBar(float CurrentHP, float MaxHP);
	
	UFUNCTION(BlueprintCallable)
	void CreateBuffIcon();
};
