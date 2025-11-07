// GRWeaponListWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRWeaponListWidget.generated.h"

class UGRWeaponSlotWidget;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRWeaponListWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UGRWeaponSlotWidget* ThirdWeapon;

	UPROPERTY(meta = (BindWidget))
	UGRWeaponSlotWidget* SecondWeapon;

	UPROPERTY(meta = (BindWidget))
	UGRWeaponSlotWidget* FirstWeapon;

	virtual void NativeConstruct() override;

public:
	void SetSelectedWeapon(int32 WeaponIndex);

	void UpdateBulletCount(int32 WeaponIndex, int32 CurrentBullets, int32 MaxBullets);

private:
	UPROPERTY()
	TArray<UGRWeaponSlotWidget*> WeaponSlots;
};
