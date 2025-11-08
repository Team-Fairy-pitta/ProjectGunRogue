#pragma once

#include "Blueprint/UserWidget.h"
#include "InputCoreTypes.h"
#include "GRKeySettingSlot.generated.h"

class UButton;
class UTextBlock;
class UInputKeySelector;
class UGRKeySettingWidget;

UCLASS()
class GUNROGUE_API UGRKeySettingSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetMappingName(const FName& InName);
	void SetActionText(const FText& InText);
	void SetKey(const FKey& InKey);
	void SetParrentWidget(UGRKeySettingWidget* InParrentWidget);

protected:
	FName MappingName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ActionText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DefaultButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInputKeySelector> InputKeySelector;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ClearButton;

	TWeakObjectPtr<UGRKeySettingWidget> ParrentWidget;

private:
	UFUNCTION()
	void OnKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void OnIsSelectingKeyChanged();

	bool bIsKeyChanged;
};
