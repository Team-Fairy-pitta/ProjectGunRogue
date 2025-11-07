// GRWeaponSlotWidget.cpp


#include "UI/GRWeaponSlotWidget.h"
#include "Components/Image.h"
#include "GRBaseTextWidget.h"

void UGRWeaponSlotWidget::SetSelected(bool bSelected)
{
	if (!BulletCountText || !BorderImage) return;
	
	bIsSelected = bSelected;

	BorderImage->SetRenderScale(bIsSelected ? FVector2D(1.2f, 1.2f) : FVector2D(1.f, 1.f));

	BulletCountText->SetVisibility(bIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UGRWeaponSlotWidget::SetBulletCount(int32 CurrentBullets, int32 MaxBullets, const FLinearColor& InColor)
{
	if (!BulletCountText) return;

	const FString BulletsText = FString::Printf(TEXT("%d / %d"), CurrentBullets, MaxBullets);

	BulletCountText->SetTextAndColor(
		FText::FromString(BulletsText),
		FSlateColor(InColor)
	);
}

void UGRWeaponSlotWidget::SetWeaponIndex(int32 Index, const FLinearColor& InColor)
{
	if (!GunIndexText) return;

	GunIndexText->SetTextAndColor(
		FText::AsNumber(Index),
		FSlateColor(InColor)
	);
}
