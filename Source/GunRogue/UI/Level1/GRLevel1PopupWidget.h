#pragma once

#include "Blueprint/UserWidget.h"
#include "GRLevel1PopupWidget.generated.h"

class UTextBlock;

UCLASS()
class GUNROGUE_API UGRLevel1PopupWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetPopupText(int32 Index, const FString& StatusString, const FString& TypeString);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> IndexText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StatusText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TypeText;
};
