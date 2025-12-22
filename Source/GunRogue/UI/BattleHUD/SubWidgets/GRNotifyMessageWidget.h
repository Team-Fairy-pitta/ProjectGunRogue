// GRNotifyMessageWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRNotifyMessageWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRNotifyMessageWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* NotifyMessageText;

	UPROPERTY(meta = (BindWidgetAnim),Transient)
	class UWidgetAnimation* Fade;

	UPROPERTY(meta = (BindWidgetAnim),Transient)
	class UWidgetAnimation* FadeIn;

	UPROPERTY(meta = (BindWidgetAnim),Transient)
	class UWidgetAnimation* FadeOut;

	UFUNCTION()
	void OnAnimFadeInFinished();

	UFUNCTION()
	void OnAnimFadeFinished();

	float MessageTime = 2.0f;
	
public:
	void SetNotifyMessage(const FText& InText, float ShowMessageTime = 2.0f);
};
