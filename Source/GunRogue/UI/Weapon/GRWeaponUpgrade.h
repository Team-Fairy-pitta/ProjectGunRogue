#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRWeaponUpgrade.generated.h"


class UButton;
class UTextBlock;
class UImage;
class AGRPlayerState;
class UBorder;


UCLASS()
class GUNROGUE_API UGRWeaponUpgrade : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:
	UFUNCTION()
	void Init(int32 GetSlotNumber);

	UFUNCTION()
	void BlindWeapon(bool bBlind);

	UFUNCTION()
	void SettingWeapon();

	UFUNCTION()
	void UpGrade();

	UFUNCTION()
	void AllRerollOption();

private:
	UFUNCTION()
	void WeaponNameUpdate(FText WeaponName);

	UFUNCTION()
	void WeaponImageUpdate(UTexture2D* Image);

	UFUNCTION()
	void WeaponLevelUpdate(int32 Level);

	UFUNCTION()
	void WeaponDamageUpdate(float Damage);

	UFUNCTION()
	void WeaponWeakpointUpdate(float Weakpoint);

	UFUNCTION()
	void WeaponLaunchspeedUpdate(float Launchspeed);

	UFUNCTION()
	void WeaponMagazineUpdate(float Magazine);

	UFUNCTION()
	void WeaponExplainUpdate(FText WeaponExplain);

	/*UFUNCTION()
	void WeaponOptionUpdate();*/


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot")
	int32 SlotNumber = 0;

	UPROPERTY(meta = (BindWidget))
	UButton* UpgradeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* RerollButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponNameText;

	UPROPERTY(meta = (BindWidget))
	UImage* WeaponIconIamge;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponLevelText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponDamageText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponWeakpointText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponLaunchspeedText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponMagazineText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponExplainText;

	//UPROPERTY(meta = (BindWidget))
	//TArray<UTextBlock> WeaponOptionListText;

	UPROPERTY(meta = (BindWidget))
	UBorder* BlindBorder;
private:
	void SetWidgetFocusable();
};
