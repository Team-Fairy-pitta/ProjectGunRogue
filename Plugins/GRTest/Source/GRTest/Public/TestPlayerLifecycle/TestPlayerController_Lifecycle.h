#pragma once

#include "CoreMinimal.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "TestPlayerController_Lifecycle.generated.h"

class ATestCharacter_Lifecycle;

UCLASS()
class GRTEST_API ATestPlayerController_Lifecycle : public AGRBattlePlayerController
{
	GENERATED_BODY()
	
public:
	ATestPlayerController_Lifecycle();

	void Spectating();

	void Respawn();
	UFUNCTION(BlueprintCallable)
	void SpectateNext();
	UFUNCTION(BlueprintCallable)
	void SpectatePrevious();

private:
	void UpdateAlivePlayerList();

	void SpectateCurrentTarget();

public:
	virtual void BeginPlay() override;

private:
	TArray<ATestCharacter_Lifecycle*> AlivePlayers;

	int32 CurrentIndex = 0;


#pragma region CheatCommands

public:
	UFUNCTION(exec)
	void PlayerDie();

	UFUNCTION(Server, Reliable)
	void ServerPlayerDie();

	UFUNCTION(exec)
	void PlayerRespawn();

	UFUNCTION(Server, Reliable)
	void ServerPlayerRespawn();

#pragma endregion
};
