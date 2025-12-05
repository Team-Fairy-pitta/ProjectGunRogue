// GRAugmentTooltipWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRAugmentTooltipWidget.generated.h"

class UTextBlock;
class UGRAugmentDefinition;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRAugmentTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetAugmentTooltip(UGRAugmentDefinition* Augment, int32 Level);
	
	void UpdateAugmentTooltipUI();

	FText SetLevelDescriptionText(int32 Index);

	FLinearColor SetLevelColors(bool bIsCurrentLevel);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Level1Text;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Level1Description;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Level2Text;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Level2Description;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Level3Text;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Level3Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Augment")
	FLinearColor CurrentLevelColor = FLinearColor::Yellow;

	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	UGRAugmentDefinition* CurrentAugment;
	
	UPROPERTY()
	TArray<UTextBlock*> LevelTexts;

	UPROPERTY()
	TArray<UTextBlock*> LevelDescriptions;
	
	int32 CurrentLevel;
};
