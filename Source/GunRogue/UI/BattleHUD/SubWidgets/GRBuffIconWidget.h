// GRBuffIcon.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRBuffIconWidget.generated.h"

class UImage;
class UTexture2D;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRBuffIconWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* BuffIconImage;

public:
	void SetBuffIcon(UTexture2D* NewIcon);
};
