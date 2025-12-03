// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRInventoryItemWidget.generated.h"

class UGRInventoryItemSlot;
class UWrapBox;
struct FGRItemHandle;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = " Inventroy")
	void UpdateInventoryDisplay();

protected:
	UPROPERTY(meta = (BindWidget))
	UWrapBox* ItemSlotsContainer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<UGRInventoryItemSlot*> AllItemSlots;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UGRInventoryItemSlot> InventorySlotClass;

private:

	UFUNCTION()
	void HandleSlotRightClick(int32 ClickedSlotIndex);
	
};
