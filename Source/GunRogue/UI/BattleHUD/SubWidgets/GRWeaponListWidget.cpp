// GRWeaponListWidget.cpp


#include "GRWeaponListWidget.h"

#include "GRWeaponSlotWidget.h"

void UGRWeaponListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WeaponSlots = { FirstWeapon, SecondWeapon, ThirdWeapon};

	for (int32 i = 0; i < WeaponSlots.Num(); i++)
	{
		if (WeaponSlots[i])
		{
			WeaponSlots[i]->SetWeaponIndex(i + 1);
			WeaponSlots[i]->SetSelected(false);
		}
	}
}

void UGRWeaponListWidget::SetSelectedWeapon(int32 WeaponIndex)
{
	for (int32 i=0; i<WeaponSlots.Num(); i++)
	{
		if (WeaponSlots[i])
		{
			WeaponSlots[i]->SetSelected( i == WeaponIndex);
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
