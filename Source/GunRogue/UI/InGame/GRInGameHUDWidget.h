#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRInGameHUDWidget.generated.h"


class UButton;


UCLASS()
class GUNROGUE_API UGRInGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	UFUNCTION()
	void OnGameOver();

	UFUNCTION()
	void OnStartLobby();

	UFUNCTION()
	void OnBackMenu();

	void SetWidgetFocusable();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> GameOverButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartLobbyButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackToMenuButton;
};
