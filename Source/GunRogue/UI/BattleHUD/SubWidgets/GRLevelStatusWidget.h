// GRLevelStatusWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRLevelStatusWidget.generated.h"

class UGRBaseTextWidget;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRLevelStatusWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UGRBaseTextWidget* CurrentLocText;

	UPROPERTY(meta = (BindWidget))
	UGRBaseTextWidget* DifficultyText;

	UPROPERTY(meta = (BindWidget))
	UGRBaseTextWidget* PlayingTimeText;

	virtual void NativeConstruct() override;

public:
	void SetCurrentLocText(const FText& InText);

	void SetDifficultyText(const FText& InText);
	
	void SetPlayingTime(int32 InSeconds);
};
