#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRWeaponUpgradeWidgetSetting.generated.h"

class UGRWeaponUpgrade;

UCLASS()
class GUNROGUE_API UGRWeaponUpgradeWidgetSetting : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

};
