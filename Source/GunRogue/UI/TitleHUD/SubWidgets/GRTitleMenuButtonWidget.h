// GRTitleMenuButtonWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRTitleMenuButtonWidget.generated.h"

class UButton;
class UTextBlock;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTitleButtonClicked);

UCLASS()
class GUNROGUE_API UGRTitleMenuButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnTitleButtonClicked OnTitleButtonClicked;
	
protected:
	UPROPERTY(meta=(BindWidget))
	UButton* MainButton;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* MainButtonText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Text")
	FText DefaultText;
	
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnMainClicked();
};
