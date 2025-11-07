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
			WeaponSlots[i]->SetWeaponIndex(i+1);
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
			WeaponSlots[i]->SetSelected((i+1) == WeaponIndex);
		}
	}
}

void UGRWeaponListWidget::UpdateBulletCount(int32 WeaponIndex, int32 CurrentBullets, int32 MaxBullets)
{
	if (WeaponSlots.IsValidIndex(WeaponIndex - 1))
	{
		if (WeaponSlots[WeaponIndex - 1])
		{
			WeaponSlots[WeaponIndex -1]->SetBulletCount(CurrentBullets, MaxBullets);
		}
	}
}
