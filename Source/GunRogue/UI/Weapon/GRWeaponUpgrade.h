#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRWeaponUpgrade.generated.h"


class UButton;
class UTextBlock;
class UImage;
class AGRPlayerState;
class UBorder;
class UScrollBox;
class UGROptionSlot;


UCLASS()
class GUNROGUE_API UGRWeaponUpgrade : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UFUNCTION()
	void Init(int32 InWeaponSlotIndex);

	UFUNCTION()
	void BlindWeapon(bool bInBlind);

	UFUNCTION()
	void SettingWeapon();

	UFUNCTION()
	void UpGrade();

	UFUNCTION()
	void RerollOption();

	UFUNCTION()
	void AllRerollOption();

private:
	UFUNCTION()
	void WeaponNameUpdate(FText InWeaponName);

	UFUNCTION()
	void WeaponImageUpdate(UTexture2D* InWeaponImage);

	UFUNCTION()
	void WeaponLevelUpdate(int32 InWeaponLevel);

	UFUNCTION()
	void WeaponDamageUpdate(float InWeaponDamage);

	UFUNCTION()
	void WeaponWeakpointUpdate(float InWeakpoint);

	UFUNCTION()
	void WeaponLaunchspeedUpdate(float InWeaponLaunchspeed);

	UFUNCTION()
	void WeaponMagazineUpdate(float InWeaponMagazine);

	UFUNCTION()
	void WeaponExplainUpdate(FText InWeaponExplain);

	UFUNCTION()
	void WeaponOptionUpdate();

	void OnOptionSelected(int32 InOptionSlotIndex);


public:

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

	UPROPERTY(meta = (BindWidget))
	UBorder* BlindBorder;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* OptionList;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGROptionSlot> OptionSlotClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot")
	int32 WeaponSlotIndex = 0;

private:
	FDelegateHandle WeaponDataUpdateHandle;	

	int32 CurrentOptionSlotIndex = -1;

	TArray<UGROptionSlot*> OptionWidgets;
};
