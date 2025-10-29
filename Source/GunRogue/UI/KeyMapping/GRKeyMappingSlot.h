#pragma once

#include "Blueprint/UserWidget.h"
#include "GRKeyMappingSlot.generated.h"

class UTextBlock;
class UButton;
class UGRKeyMappingWidget;

UCLASS()
class GUNROGUE_API UGRKeyMappingSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UGRKeyMappingSlot(const FObjectInitializer& ObjectInitializer);
	void Init(const FName& InActionName, const FName& InKeyName, UGRKeyMappingWidget* Parent);

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	void FocusChangeButton();
	void UnfocusChangeButton();

protected:
	FName SlotActionName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ActionNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> KeyNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MappingChangeButton;

	UPROPERTY()
	TObjectPtr<UGRKeyMappingWidget> ParentWidget;

	UFUNCTION()
	void OnChangeButtonClicked();
};
