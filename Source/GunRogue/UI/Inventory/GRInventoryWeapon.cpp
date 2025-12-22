// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/GRInventoryWeapon.h"

#include "Components/Button.h"
#include "Player/GRPlayerState.h"

void UGRInventoryWeapon::WeaponDrop()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AGRPlayerState* GRPS = PC->GetPlayerState<AGRPlayerState>())
		{
			GRPS->DropWeapon(WeaponSlotIndex);
			BlindWeapon(true);
		}
		
			UE_LOG(LogTemp, Warning, TEXT("가져옴"));
	}
}

void UGRInventoryWeapon::NativeConstruct()
{
	Super::NativeConstruct();

	if (DropButton)
	{
		DropButton->OnClicked.AddUniqueDynamic(this, &UGRInventoryWeapon::WeaponDrop);
	}
}

void UGRInventoryWeapon::NativeDestruct()
{

	if (DropButton)
	{
		DropButton->OnClicked.RemoveDynamic(this, &UGRInventoryWeapon::WeaponDrop);
	}

	
	Super::NativeDestruct();
}
