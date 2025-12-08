#pragma once

#include "Player/GRPlayerController.h"
#include "GameModes/Lobby/GRLobbyGameState.h"
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
	void HideLobbyWidget();

	void StartGame();

	void UpdateHostPlayerInfo(APlayerState* HostPlayer);
	void UpdateGuestPlayersInfo(TArray<FGuestPlayer>& GuestPlayers);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRLobbyHUDWidget> LobbyWidgetClass;

	UPROPERTY()
	TObjectPtr<UGRLobbyHUDWidget> LobbyWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|Map")
	TSoftObjectPtr<UWorld> GameStartMap;

	UFUNCTION(Server, Reliable)
	void ServerRPC_StartGame();

private:
	void CreateWidgets();
};
