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
	UGRBaseTextWidget(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable, Category="Text")
	void UpdateTextAndFont(const FText& InText, const FSlateFontInfo& InFontInfo, const FSlateColor& InTextColor);

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
