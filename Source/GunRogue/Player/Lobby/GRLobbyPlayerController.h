#pragma once

#include "Player/GRPlayerController.h"
#include "GameModes/Lobby/GRLobbyGameState.h"
#include "GRLobbyPlayerController.generated.h"

class UGRLobbyHUDWidget;
class UGRPerkHUDWidget;
class AGRCharacter;

UCLASS()
class GUNROGUE_API AGRLobbyPlayerController : public AGRPlayerController
{
	GENERATED_BODY()

public:
	AGRLobbyPlayerController();
	virtual void BeginPlay() override;

	void ShowLobbyWidget();
	void HideLobbyWidget();

	void ShowLoadingWidget();
	void HideLoadingWidget();

	void StartGame();

	void UpdateHostPlayerInfo(FHostPlayer& HostPlayer);
	void UpdateGuestPlayersInfo(TArray<FGuestPlayer>& GuestPlayers);

	UFUNCTION(Server, Reliable)
	void ServerRPC_Ready();

	UFUNCTION(Server, Reliable)
	void ServerRPC_CancelReady();

	UFUNCTION(Server, Reliable)
	void ServerRPC_SelectCharacter(int32 CharacterIndex);

	UFUNCTION(Client, Reliable)
	void ClientRPC_OnConfirmReady();

	UFUNCTION(Client, Reliable)
	void ClientRPC_OnConfirmCancelReady();

	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowLoadingWidget();

	void UpdateCanStartGame(bool bCanStart);
	bool IsAllPlayerReady();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|CharacterClass")
	TArray<TSubclassOf<AGRCharacter>> PlayableCharacterClasses;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRLobbyHUDWidget> LobbyWidgetClass;

	UPROPERTY()
	TObjectPtr<UGRLobbyHUDWidget> LobbyWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UUserWidget> LoadingWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> LoadingWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|Map")
	TSoftObjectPtr<UWorld> GameStartMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|CharacterSpawn")
	TArray<FTransform> CharacterSpawnTransformsInLobby;

	UFUNCTION(Server, Reliable)
	void ServerRPC_StartGame();

private:
	void CreateWidgets();
	AActor* SpawnCharacterInLobby(int32 Index, TSubclassOf<AGRCharacter> CharacterClass);

	UPROPERTY()
	TObjectPtr<AActor> SpawnedLobbyCharacter;

#pragma region Perk
public:
	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowPerkWidget();
	
	UFUNCTION(BlueprintCallable)
	void ShowPerkWidget();

	UFUNCTION(BlueprintCallable)
	void HidePerkWidget();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRPerkHUDWidget> PerkWidgetClass;

	UPROPERTY()
	TObjectPtr<UGRPerkHUDWidget> PerkWidgetInstance;

#pragma endregion
	
};
