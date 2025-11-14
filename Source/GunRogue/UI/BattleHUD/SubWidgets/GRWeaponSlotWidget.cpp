// GRWeaponSlotWidget.cpp


#include "GRWeaponSlotWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void UGRWeaponSlotWidget::SetSelected(bool bSelected)
{
	if (!BulletCountText || !WeaponSlotBorder)
	{
		return;
	}
	
	bIsSelected = bSelected;
	
	FVector2D NewScale = FVector2D::One();
	ESlateVisibility NewVisibility = ESlateVisibility::Collapsed;

	if (bIsSelected)
	{
		float SelectedScale = 1.2f;
		NewScale *= SelectedScale;
		NewVisibility = ESlateVisibility::Visible;
	}

	WeaponSlotBorder->SetRenderScale(NewScale);
	BulletCountText->SetVisibility(NewVisibility);
}

void UGRWeaponSlotWidget::SetBulletCount(int32 CurrentBullets, int32 MaxBullets)
{
	if (!BulletCountText)
	{
		return;
	}

	const FString BulletsText = FString::Printf(TEXT("%d / %d"), CurrentBullets, MaxBullets);

	BulletCountText->SetText(FText::FromString(BulletsText));
}

void UGRWeaponSlotWidget::SetWeaponIndex(int32 Index)
{
	if (!GunIndexText)
	{
		return;
	}

	GunIndexText->SetText(FText::AsNumber(Index));
}
