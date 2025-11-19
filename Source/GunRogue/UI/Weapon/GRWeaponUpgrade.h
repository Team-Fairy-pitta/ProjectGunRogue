#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRWeaponUpgrade.generated.h"

class UButton;


/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRWeaponUpgrade : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	UButton* UpgradeButton;


protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleClicked();
};
