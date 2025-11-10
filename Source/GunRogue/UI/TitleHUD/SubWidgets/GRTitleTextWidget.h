// GRTitleTextWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRTitleTextWidget.generated.h"

class UGRBaseTextWidget;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRTitleTextWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UGRBaseTextWidget* GameTitleText;
};
