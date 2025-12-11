#pragma once

#include "CoreMinimal.h"
#include "GameModes/GRGameState.h"
#include "Character/GRCharacter.h"
#include "GRLobbyGameState.generated.h"

class AGRLobbyPlayerState;

USTRUCT()
struct FGuestPlayer
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AGRLobbyPlayerState> PlayerState = nullptr;

	UPROPERTY()
	TSubclassOf<AGRCharacter> SelectedCharacterClass = nullptr;

	UPROPERTY()
	int8 bIsReady = 0;
};

USTRUCT()
struct FHostPlayer
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AGRLobbyPlayerState> PlayerState = nullptr;

	UPROPERTY()
	TSubclassOf<AGRCharacter> SelectedCharacterClass = nullptr;
};

UCLASS()
class GUNROGUE_API AGRLobbyGameState : public AGRGameState
{
	GENERATED_BODY()
	
public:
	AGRLobbyGameState();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnRep_ReplicatedHasBegunPlay() override;

	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

	void Ready(APlayerState* GuestPlayerState);
	void CancelReady(APlayerState* GuestPlayerState);
	void SelectCharacterClass(APlayerState* PlayerState, TSubclassOf<AGRCharacter> SelectedCharacterClass);

	bool IsAllPlayerReady();

	UFUNCTION()
	void OnRep_HostPlayer();

	UFUNCTION()
	void OnRep_GuestPlayers();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_HostPlayer)
	FHostPlayer HostPlayer;

	UPROPERTY(ReplicatedUsing = OnRep_GuestPlayers)
	TArray<FGuestPlayer> GuestPlayers;

	void OnRep_HostPlayer_NextTick();
	void OnRep_GuestPlayers_NextTick();

	void UpdateCanStartGame();

	void RegistPlayerIndex(APlayerController* PlayerController);

private:
	int32 FindGuestIndex(APlayerState* GuestPlayerState);
};
