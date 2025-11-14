// GRLobbyPlayerListWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRLobbyPlayerListWidget.generated.h"

class UGRLobbyPlayerSlotWidget;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRLobbyPlayerListWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* LobbyPlayerContainer;

	UPROPERTY(EditDefaultsOnly, Category="LobbyWidget")
	TSubclassOf<UGRLobbyPlayerSlotWidget> LobbyPlayerSlotClass;

	UPROPERTY()
	TArray<UGRLobbyPlayerSlotWidget*> LobbyPlayerSlots;

public:
	UFUNCTION(BlueprintCallable)
	void CreateLobbyPlayerSlot();
};
