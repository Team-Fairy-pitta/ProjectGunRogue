#pragma once

#include "Blueprint/UserWidget.h"
#include "GRKeySettingSlot.generated.h"

class UButton;
class UTextBlock;
class UInputKeySelector;

UCLASS()
class GUNROGUE_API UGRKeySettingSlot : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ActionText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DefaultButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInputKeySelector> InputKeySelector;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ClearButton;
};
