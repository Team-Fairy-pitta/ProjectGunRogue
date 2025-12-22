// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Weapon/GRWeaponUpgrade.h"
#include "GRInventoryWeapon.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRInventoryWeapon : public UGRWeaponUpgrade
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UButton* DropButton;

	UFUNCTION()
	void WeaponDrop();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
};
