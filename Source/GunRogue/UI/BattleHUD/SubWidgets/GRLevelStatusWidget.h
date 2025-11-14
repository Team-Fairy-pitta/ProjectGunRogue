// GRLevelStatusWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRLevelStatusWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRLevelStatusWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentLocText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DifficultyText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayingTimeText;

	virtual void NativeConstruct() override;

public:
	void SetCurrentLocText(const FText& InText);

	void SetDifficultyText(const FText& InText);
	
	void SetPlayingTime(int32 InSeconds);
};
