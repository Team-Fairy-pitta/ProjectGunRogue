#pragma once

#include "CoreMinimal.h"
#include "GameModes/GRGameState.h"
#include "GRLobbyGameState.generated.h"

class APlayerState;

USTRUCT()
struct FGuestPlayer
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<APlayerState> GuestPlayerState = nullptr;

	UPROPERTY()
	int8 bIsReady = 0;
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

protected:
	UPROPERTY(ReplicatedUsing = OnRep_HostPlayerState)
	TObjectPtr<APlayerState> HostPlayerState;

	UPROPERTY(ReplicatedUsing = OnRep_GuestPlayerStates)
	TArray<FGuestPlayer> GuestPlayerStates;

	UFUNCTION()
	void OnRep_HostPlayerState();

	UFUNCTION()
	void OnRep_GuestPlayerStates();
};
