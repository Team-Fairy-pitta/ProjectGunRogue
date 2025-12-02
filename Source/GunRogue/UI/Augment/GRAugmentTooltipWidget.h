// GRAugmentTooltipWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRAugmentTooltipWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRAugmentTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

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
};
