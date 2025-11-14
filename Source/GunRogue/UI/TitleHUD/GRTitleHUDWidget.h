// GRTitleHUDWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRTitleHUDWidget.generated.h"

class UGRTitleMenuButtonWidget;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRTitleHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UGRTitleMenuButtonWidget* StartGameButton;

	UPROPERTY(meta = (BindWidget))
	UGRTitleMenuButtonWidget* CheckInvitationButton;

	UPROPERTY(meta = (BindWidget))
	UGRTitleMenuButtonWidget* SetGameButton;

	UPROPERTY(meta = (BindWidget))
	UGRTitleMenuButtonWidget* ExitGameButton;

	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnStartGameClicked();

	UFUNCTION()
	void OnCheckInvitationClicked();

	UFUNCTION()
	void OnSetGameClicked();

	UFUNCTION()
	void OnExitGameClicked();
};
