// GRWeaponSlotWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRWeaponSlotWidget.generated.h"

class UTextBlock;
class UBorder;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRWeaponSlotWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* BulletCountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GunIndexText;

	UPROPERTY(meta = (BindWidget))
	UBorder* WeaponSlotBorder;

public:
	void SetSelected(bool bSelected);

	void SetBulletCount(int32 CurrentBullets, int32 MaxBullets);

	void SetWeaponIndex(int32 Index);

private:
	bool bIsSelected = false;
};
