// GRGoodsTextWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRGoodsTextWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRGoodsTextWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* GoldCountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GemCountText;

public:
	void SetGoldCountText(int32 InCount);

	void SetGemCountText(int32 InCount);
};
