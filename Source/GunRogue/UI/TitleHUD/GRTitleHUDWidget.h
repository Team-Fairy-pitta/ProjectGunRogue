#pragma once

#include "Blueprint/UserWidget.h"
#include "GRTitleHUDWidget.generated.h"

class UGRTitleMenuButtonWidget;

UCLASS()
class GUNROGUE_API UGRTitleHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGRTitleMenuButtonWidget> StartGameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGRTitleMenuButtonWidget> CheckInvitationButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGRTitleMenuButtonWidget> SetGameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGRTitleMenuButtonWidget> ExitGameButton;

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
