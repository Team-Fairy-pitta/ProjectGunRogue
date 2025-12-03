// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRInventoryItemSlot.generated.h"

class UTextBlock;
class UButton;
class UImage;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotContextMenu, int32, SlotIndex);

UCLASS()
class GUNROGUE_API UGRInventoryItemSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSlot(UTexture2D* ItemImage, FText ItemName, FText ItemDescription);

	UPROPERTY(blueprintReadWrite, Category="Slot")
	int32 SlotIndex = -1;

	void SetSlotIndex(int32 Index) { SlotIndex = Index; }

	void ClearSlot();

	UPROPERTY(BlueprintAssignable, Category = "Slot|Events")
	FOnSlotContextMenu OnSlotRightClicked;

protected:

	UPROPERTY(meta = (BindWidget))
	UImage* ItemSlotIcon;

	UPROPERTY(meta = (BindWidget))
	UButton* ItemSlotButton;

	UPROPERTY(meta = (BindWidget))
	UImage* HoverImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HoverName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HoverDescription;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	
};
