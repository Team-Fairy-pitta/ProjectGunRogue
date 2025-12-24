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
	UTextBlock* PlayingTimeText;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void UpdateGamePlayingTime();

	FTimerHandle GamePlayTimer;

public:
	void SetCurrentLocText(const FText& InText);
	
	void SetPlayingTime(int32 InSeconds);
};
