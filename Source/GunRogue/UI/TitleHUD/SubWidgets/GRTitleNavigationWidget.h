// GRTitleNavigationWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRTitleNavigationWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRTitleNavigationWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* StartGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CheckInvitationButton;

	UPROPERTY(meta = (BindWidget))
	UButton* SetGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitGameButton;

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
