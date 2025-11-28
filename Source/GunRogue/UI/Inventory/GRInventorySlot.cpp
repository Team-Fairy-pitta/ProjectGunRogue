// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/GRInventorySlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UGRInventorySlot::SetSlot(UTexture2D* ItemImage, FText ItemName, FText ItemDescription)
{
	if (ItemSlotIcon)
	{
		ItemSlotIcon->SetBrushFromTexture(ItemImage);
	}

	if (HoverImage)
	{
		HoverImage->SetBrushFromTexture(ItemImage);
	}

	if (HoverName)
	{
		HoverName->SetText(ItemName);
	}

	if (HoverDescription)
	{
		HoverDescription->SetText(ItemDescription);
	}
	
}

void UGRInventorySlot::ClearSlot()
{
	if (ItemSlotIcon)
	{
		ItemSlotIcon->SetBrushFromTexture(nullptr);
	}
	SlotIndex = -1;
}

FReply UGRInventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (SlotIndex != -1)
		{
			OnSlotRightClicked.Broadcast(SlotIndex);

			return FReply::Handled();
		}
	}

	return Reply;
}
