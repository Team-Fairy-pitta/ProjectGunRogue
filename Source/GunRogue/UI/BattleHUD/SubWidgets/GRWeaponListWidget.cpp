// GRWeaponListWidget.cpp


#include "GRWeaponListWidget.h"

#include "GRWeaponSlotWidget.h"

void UGRWeaponListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WeaponSlots = { FirstWeapon, SecondWeapon};

	for (int32 i = 0; i < WeaponSlots.Num(); i++)
	{
		if (WeaponSlots[i])
		{
			WeaponSlots[i]->SetWeaponIndex(i + 1);
			WeaponSlots[i]->SetSelected(false);
			WeaponSlots[i]->SetDisable();
		}
	}
}

void UGRWeaponListWidget::SetSelectedWeapon(int32 WeaponIndex)
{
	for (int32 i=0; i < WeaponSlots.Num(); i++)
	{
		if (WeaponSlots[i])
		{
			bool bIsSelected = i == WeaponIndex;
			WeaponSlots[i]->SetSelected(bIsSelected);
		}
	}
}

void UGRWeaponListWidget::UpdateBulletCount(int32 WeaponIndex, int32 CurrentBullets, int32 MaxBullets)
{
	if (WeaponSlots.IsValidIndex(WeaponIndex))
	{
		if (WeaponSlots[WeaponIndex])
		{
			WeaponSlots[WeaponIndex]->SetBulletCount(CurrentBullets, MaxBullets);
		}
	}
}

void UGRWeaponListWidget::UpdateWeaponImage(int32 WeaponIndex, UTexture2D* WeaponImage)
{
	if (WeaponSlots.IsValidIndex(WeaponIndex))
	{
		if (WeaponSlots[WeaponIndex])
		{
			WeaponSlots[WeaponIndex]->SetWeaponImage(WeaponImage);
		}
	}
}

void UGRWeaponListWidget::EnableWeaponSlot(int32 WeaponIndex)
{
	if (WeaponSlots.IsValidIndex(WeaponIndex))
	{
		if (WeaponSlots[WeaponIndex])
		{
			WeaponSlots[WeaponIndex]->SetEnable();
		}
	}
}

void UGRWeaponListWidget::DisableWeaponSlot(int32 WeaponIndex)
{
	if (WeaponSlots.IsValidIndex(WeaponIndex))
	{
		if (WeaponSlots[WeaponIndex])
		{
			WeaponSlots[WeaponIndex]->SetDisable();
		}
	}
}
