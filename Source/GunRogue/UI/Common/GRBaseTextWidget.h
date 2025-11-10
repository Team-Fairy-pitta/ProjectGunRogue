// GRBaseTextWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRBaseTextWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRBaseTextWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Text")
	void SetText(const FText& InText);

	UFUNCTION(BlueprintCallable, Category="Text")
	void SetColor(const FSlateColor& InTextColor);
	
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Text")
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Text")
	FSlateFontInfo FontInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Text")
	FSlateColor TextColor;

	virtual void NativeConstruct() override;

private:
	void SyncTextAndFont();
};
