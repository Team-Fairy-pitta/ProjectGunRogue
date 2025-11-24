// GRPerkTooltipWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRPerkTooltipWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRPerkTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PerkTooltipHeader;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ValuePerLevelText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentPerkLevelText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NextPerkLevelText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CostPerLevelText;
	
	virtual void NativeConstruct() override;

public:
	void SetPerkTooltipHeader(const FText& InText);

	void SetValuePerLevelText(const FText& InText, float InValue);

	void SetCurrentPerkLevelText(const FText& InText, float InValue, int32 Level);

	void SetNextPerkLevelText(const FText& InText, float InValue, int32 Level, int32 MaxLevel);

	void SetCostPerLevelText(int32 Level, int32 Cost, int32 MaxLevel);
	
};
