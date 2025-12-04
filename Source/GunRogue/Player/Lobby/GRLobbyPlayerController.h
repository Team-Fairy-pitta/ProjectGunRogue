#pragma once

#include "Player/GRPlayerController.h"
#include "GRLobbyPlayerController.generated.h"

class UGRLobbyHUDWidget;

UCLASS()
class GUNROGUE_API AGRLobbyPlayerController : public AGRPlayerController
{
	GENERATED_BODY()

public:
	AGRLobbyPlayerController();
	virtual void BeginPlay() override;

	void ShowLobbyWidget();

	void ShowESCMenuWidget();
	void HideESCMenuWidget();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRLobbyHUDWidget> LobbyWidgetClass;

	UPROPERTY()
	TObjectPtr<UGRLobbyHUDWidget> LobbyWidgetInstance;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	//TSubclassOf<UGRSettingWidget> SettingWidgetClass;

	//UPROPERTY()
	//TObjectPtr<UGRSettingWidget> SettingWidgetInstance;

private:
	void CreateWidgets();
};
