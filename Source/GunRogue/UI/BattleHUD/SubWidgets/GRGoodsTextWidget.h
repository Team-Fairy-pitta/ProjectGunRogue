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
	UTextBlock* GoodsNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GoodsCountText;

public:
	void SetGoodsNameText(const FText& InText);

	void SetGoodsCountText(int32 InCount);
};
