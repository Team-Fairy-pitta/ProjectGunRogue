// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRInventoryAugmentWidget.generated.h"

class UGRInventoryAugmentSlot;
class UWrapBox;
class UHorizontalBox;
class UGRAugmentSlotWidget;
class UGRAugmentTooltipWidget;
class UTextBlock;

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRInventoryAugmentWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UWrapBox* AugmentContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Augment")
	TSubclassOf<UGRInventoryAugmentSlot> AugmentSlotClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Augment")
	TSubclassOf<UGRAugmentTooltipWidget> AugmentTooltipClass;

	UPROPERTY()
	UGRAugmentTooltipWidget* AugmentTooltipWidget;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
	
public:

	void UpdateTooltip(UGRInventoryAugmentSlot* AugmentSlot);
	
	UFUNCTION()
	void ShowTooltip(UGRInventoryAugmentSlot* AugmentSlot);

	UFUNCTION()
	void HideTooltip(UGRInventoryAugmentSlot* AugmentSlot);

private:
	void CreateAugmentSlot();
	
};
