// GRWeaponSlotWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRWeaponSlotWidget.generated.h"

class UTextBlock;
class UBorder;
class UImage;
class UTexture2D;
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

	UPROPERTY(meta = (BindWidget))
	UImage* GunImage;

public:
	void SetSelected(bool bSelected);

	void SetBulletCount(int32 CurrentBullets, int32 MaxBullets);

	void SetWeaponIndex(int32 Index);

	void SetWeaponImage(UTexture2D* WeaponImage);

	void SetEnable();
	void SetDisable();

private:
	bool bIsSelected = false;
};
