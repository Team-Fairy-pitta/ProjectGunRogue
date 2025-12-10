// GRAugmentSlotWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRAugmentSlotWidget.generated.h"

class UTextBlock;
class UBorder;
class UImage;
class UProgressBar;
class UButton;
class UGRAugmentDefinition;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAugmentSlotHovered, UGRAugmentSlotWidget*, AugmentSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAugmentSlotUnhovered, UGRAugmentSlotWidget*, AugmentSlot);

UCLASS()
class GUNROGUE_API UGRAugmentSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnAugmentSlotHovered OnAugmentSlotHovered;

	UPROPERTY(BlueprintAssignable)
	FOnAugmentSlotUnhovered OnAugmentSlotUnhovered;
	
	void SetAugmentSlot(UGRAugmentDefinition* Augment, int32 Level);

	void UpdateAugmentSlotUI();

	void SetSlotName();

	void SetSlotIcon();

	void SetSlotLevelBar();

	void SetSlotDescription();

	void SetSlotCategory();

	UGRAugmentDefinition* GetCurrentAugment() const { return CurrentAugment; }

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* SlotButton;
	
	UPROPERTY(meta = (BindWidget))
	UBorder* SlotBackground;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SlotName;

	UPROPERTY(meta = (BindWidget))
	UImage* SlotIcon;
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* SlotLevelBar;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SlotDescription;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SlotCategory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Augment")
	FLinearColor HoveredBrushColor;

	virtual void NativeConstruct() override;

private:
	FLinearColor DefaultBrushColor;
	
	UPROPERTY()
	UGRAugmentDefinition* CurrentAugment = nullptr;

	int32 CurrentLevel;
	
	UFUNCTION()
	void OnSlotHovered();

	UFUNCTION()
	void OnSlotUnhovered();

	UFUNCTION()
	void OnSlotClicked();
};
