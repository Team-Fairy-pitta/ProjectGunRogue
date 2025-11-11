// GRTitleHUDWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRTitleHUDWidget.generated.h"

class UGRTitleNavigationWidget;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRTitleHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UGRTitleNavigationWidget* TitleNavigation;
};
