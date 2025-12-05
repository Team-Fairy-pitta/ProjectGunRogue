#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GROptionSlot.generated.h"


class UButton;
class UTextBlock;
struct FWeaponOption;

DECLARE_DELEGATE_OneParam(FOnOptionClicked, int32);

UCLASS()
class GUNROGUE_API UGROptionSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetSelected(bool bSelected);

	UFUNCTION()
	void HandleClick();

	void InitSlot(int32 InSlotIndex, const FWeaponOption& InOptionData);
	
public:
	UPROPERTY(meta = (BindWidget))
	UButton* SelectButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* OptionText;

	int32 OptionSlotIndex = 0;

	FOnOptionClicked OnOptionClicked;

};
