#pragma once

#include "Blueprint/UserWidget.h"
#include "GRItemInfoWidget.generated.h"

class UTextBlock;
class UImage;
class UTexture2D;

UCLASS()
class GUNROGUE_API UGRItemInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitItemInfo(UTexture2D* Icon, const FText& Name, const FText& Description);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemDescriptionText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIconImage;
};
