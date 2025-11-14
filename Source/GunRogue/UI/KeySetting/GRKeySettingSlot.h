#pragma once

#include "Blueprint/UserWidget.h"
#include "InputCoreTypes.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "GRKeySettingSlot.generated.h"

class UButton;
class UTextBlock;
class UInputKeySelector;
class UGRKeySettingWidget;
struct FPlayerKeyMapping;

UCLASS()
class GUNROGUE_API UGRKeySettingSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetKeyMapping(const FPlayerKeyMapping& InMapping);
	void SetParentWidget(UGRKeySettingWidget* InParentWidget);

protected:
	void SetMappingName(const FName& InName);
	void SetActionText(const FText& InText);
	void SetKey(const FKey& InKey);
	void SetDefaultButtonVisibility(const FKey& InCurrentKey);

	void NotifyToParent(const FKey& NewKey);

	FPlayerKeyMapping Mapping;
	FName MappingName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ActionText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DefaultButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInputKeySelector> InputKeySelector;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ClearButton;

	TWeakObjectPtr<UGRKeySettingWidget> ParentWidget;

private:
	UFUNCTION()
	void OnKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void OnIsSelectingKeyChanged();

	UFUNCTION()
	void OnDefaultButtonClicked();

	UFUNCTION()
	void OnClearButtonClicked();

	bool bIsKeyChanged;
};
