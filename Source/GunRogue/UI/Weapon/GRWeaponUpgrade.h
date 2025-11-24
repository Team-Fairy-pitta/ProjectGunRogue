#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRWeaponUpgrade.generated.h"

class UButton;
class UTextBlock;
class UImage;


UCLASS()
class GUNROGUE_API UGRWeaponUpgrade : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UFUNCTION()
	void SettingWeapon();

	UFUNCTION()
	void UpGrade();

	UFUNCTION()
	void Reroll();

private:
	UFUNCTION()
	void WeaponNameUpdate(FString WeaponName);

	/*UFUNCTION()
	void WeaponImageUpdate();*/

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
	void WeaponExplainUpdate(FString WeaponExplain);

	/*UFUNCTION()
	void WeaponOptionUpdate();*/
	
public:

	UPROPERTY(meta = (BindWidget))
	UButton* UpgradeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* RerollButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponNameText;

	/*UPROPERTY(meta = (BindWidget))
	UImage* WeaponImage;*/

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
};
