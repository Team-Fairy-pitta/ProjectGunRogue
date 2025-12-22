#pragma once

#include "Blueprint/UserWidget.h"
#include "GRSpectatorHUDWidget.generated.h"

class UTextBlock;

UCLASS()
class GUNROGUE_API UGRSpectatorHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetTargetPlayerName(const FString& InString);
	void SetTargetPlayerName(const FText& InText);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TargetPlayerNameText;
};
