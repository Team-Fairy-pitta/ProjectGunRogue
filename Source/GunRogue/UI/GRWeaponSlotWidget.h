// GRWeaponSlotWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRWeaponSlotWidget.generated.h"

class UGRBaseTextWidget;
class UImage;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRWeaponSlotWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UGRBaseTextWidget* BulletCountText;

	UPROPERTY(meta = (BindWidget))
	UGRBaseTextWidget* GunIndexText;

	UPROPERTY(meta = (BindWidget))
	UImage* BorderImage;

public:
	void SetSelected(bool bSelected);

	void SetBulletCount(int32 CurrentBullets, int32 MaxBullets, const FLinearColor& InColor = FLinearColor::White);

	void SetWeaponIndex(int32 Index, const FLinearColor& InColor = FLinearColor::White);

private:
	bool bIsSelected = false;
};
