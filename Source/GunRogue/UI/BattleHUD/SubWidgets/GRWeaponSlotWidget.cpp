// GRWeaponSlotWidget.cpp


#include "GRWeaponSlotWidget.h"
#include "Components/Border.h"
#include "UI/Common/GRBaseTextWidget.h"

void UGRWeaponSlotWidget::SetSelected(bool bSelected)
{
	if (!BulletCountText || !WeaponSlotBorder) return;
	
	bIsSelected = bSelected;

	WeaponSlotBorder->SetRenderScale(bIsSelected ? FVector2D(1.2f, 1.2f) : FVector2D(1.f, 1.f));

	BulletCountText->SetVisibility(bIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UGRWeaponSlotWidget::SetBulletCount(int32 CurrentBullets, int32 MaxBullets)
{
	if (!BulletCountText) return;

	const FString BulletsText = FString::Printf(TEXT("%d / %d"), CurrentBullets, MaxBullets);

	BulletCountText->SetText(FText::FromString(BulletsText));
}

void UGRWeaponSlotWidget::SetWeaponIndex(int32 Index)
{
	if (!GunIndexText) return;

	GunIndexText->SetText(FText::AsNumber(Index));
}
