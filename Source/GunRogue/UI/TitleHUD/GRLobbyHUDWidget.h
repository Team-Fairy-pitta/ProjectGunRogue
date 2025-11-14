// GRLobbyHUDWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRLobbyHUDWidget.generated.h"

class UGRLobbyCharacterSelectSlotWidget;
class UGRLobbyPlayerListWidget;
class UGRLobbyBaseButtonWidget;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRLobbyHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UGRLobbyPlayerListWidget* GetLobbyPlayerListWidget() const { return LobbyPlayerList; }
	
protected:
	UPROPERTY(meta = (BindWidget))
	UGRLobbyPlayerListWidget* LobbyPlayerList;

	UPROPERTY(meta = (BindWidget))
	UGRLobbyCharacterSelectSlotWidget* FirstCharacterSlot;

	UPROPERTY(meta = (BindWidget))
	UGRLobbyCharacterSelectSlotWidget* SecondCharacterSlot;

	UPROPERTY(meta = (BindWidget))
	UGRLobbyCharacterSelectSlotWidget* ThirdCharacterSlot;

	UPROPERTY(meta = (BindWidget))
	UGRLobbyCharacterSelectSlotWidget* FourthCharacterSlot;

	UPROPERTY(meta = (BindWidget))
	UGRLobbyCharacterSelectSlotWidget* FifthCharacterSlot;

	UPROPERTY(meta = (BindWidget))
	UGRLobbyCharacterSelectSlotWidget* SixthCharacterSlot;

	UPROPERTY(meta = (BindWidget))
	UGRLobbyBaseButtonWidget* PlayerInfoButton;

	UPROPERTY(meta = (BindWidget))
	UGRLobbyBaseButtonWidget* PlayerPerksButton;

	UPROPERTY(meta = (BindWidget))
	UGRLobbyBaseButtonWidget* StartGameButton;

	UPROPERTY(meta = (BindWidget))
	UGRLobbyBaseButtonWidget* ReadyGameButton;

	UPROPERTY(meta = (BindWidget))
	UGRLobbyBaseButtonWidget* ExitLobbyButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	UGRLobbyCharacterSelectSlotWidget* CurrentClickedSlot = nullptr;

	UPROPERTY()
	TArray<UGRLobbyCharacterSelectSlotWidget*> CharacterSlots;
	
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
	void UpdateLobbyButtonVisibility(bool bHost);
	
private:
	UFUNCTION()
	void OnCharacterSelected(int32 SelectedIndex);

	UFUNCTION()
	void OnPlayerInfoClicked();

	UFUNCTION()
	void OnPlayerPerksClicked();

	UFUNCTION()
	void OnStartGameClicked();

	UFUNCTION()
	void OnReadyGameClicked();

	UFUNCTION()
	void OnExitLobbyClicked();
};
