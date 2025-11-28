#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GROptionSlot.generated.h"


class UButton;
class UBorder;
class UTextBlock;

DECLARE_DELEGATE_OneParam(FOnOptionClicked, int32);

UCLASS()
class GUNROGUE_API UGROptionSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetSelected(bool bSelected);

	UFUNCTION()
	void HandleClick();
	
public:
	UPROPERTY(meta = (BindWidget))
	UButton* SelectButton;

	UPROPERTY(meta = (BindWidget))
	UBorder* SelectBorder;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* OptionText;

	int32 OptionSlotIndex = 0;

	FOnOptionClicked OnOptionClicked;


};
