// GRLobbyPlayerSlotWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRLobbyPlayerSlotWidget.generated.h"

class UImage;
class UGRBaseTextWidget;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRLobbyPlayerSlotWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* PlayerIcon;

	UPROPERTY(meta = (BindWidget))
	UGRBaseTextWidget* PlayerNameText;

public:
	void SetPlayerNameText(const FText& InText);
};
