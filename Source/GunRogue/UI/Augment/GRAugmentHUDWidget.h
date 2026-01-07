// GRAugmentHUDWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRAugmentHUDWidget.generated.h"

class UHorizontalBox;
class UGRAugmentSlotWidget;
class UGRAugmentTooltipWidget;
class UTextBlock;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRAugmentHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CharacterName;
	
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* AugmentContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Augment")
	TSubclassOf<UGRAugmentSlotWidget> AugmentSlotClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Augment")
	TSubclassOf<UGRAugmentTooltipWidget> AugmentTooltipClass;

	UPROPERTY()
	UGRAugmentTooltipWidget* AugmentTooltipWidget;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
	
public:
	void SetCharacterName();

	void UpdateTooltip(UGRAugmentSlotWidget* AugmentSlot);
	
	UFUNCTION()
	void ShowTooltip(UGRAugmentSlotWidget* AugmentSlot);

	UFUNCTION()
	void HideTooltip(UGRAugmentSlotWidget* AugmentSlot);

private:
	void SetWidgetFocusable();
	void CreateAugmentSlot();
};
