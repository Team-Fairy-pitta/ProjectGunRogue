// GRGoodsTextWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRGoodsTextWidget.generated.h"

class UGRBaseTextWidget;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRGoodsTextWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UGRBaseTextWidget* GoodsNameText;

	UPROPERTY(meta = (BindWidget))
	UGRBaseTextWidget* GoodsCountText;

public:
	void SetGoodsNameText(const FText& InText, const FLinearColor& InColor = FLinearColor::White);

	void SetGoodsCountText(int32 InCount, const FLinearColor& InColor = FLinearColor::White);
};
