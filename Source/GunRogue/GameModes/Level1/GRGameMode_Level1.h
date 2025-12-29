#pragma once

#include "GameModes/GRGameMode.h"
#include "GameModes/Level1/GRLevel1Data.h"
#include "GRGameMode_Level1.generated.h"

class AGRLevel1ControlPanel;

UCLASS()
class GUNROGUE_API AGRGameMode_Level1 : public AGRGameMode
{
	GENERATED_BODY()
	
	friend struct FGRLevel1Data;

public:
	virtual void BeginPlay() override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	
	bool TryRespawnPlayer(AController* TargetPlayer, AActor* AlivePlayer);
	void TryRespawnAllDeadPlayers();

	FGRLevel1Node* GetLevel1Node(int32 Index);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue")
	TArray<TSoftObjectPtr<UWorld>> RandomLevelPool;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue")
	TSoftObjectPtr<UWorld> BossLevel;

	UPROPERTY()
	FGRLevel1Data Level1Data;

	void ReceiveSpawnEnemy();
	void ReceiveDestroyEnemy();

	bool CheckGameOver();
	void GameOver();

	void AddLevel1ControlPanel(AGRLevel1ControlPanel* Level1ControlPanel);
	void RemoveLevel1ControlPanel(AGRLevel1ControlPanel* Level1ControlPanel);

	UFUNCTION(BlueprintCallable)
	void BroadcastNotifyMessage(const FText& Message, float ShowMessageTime);

	void BroadcastClearMessage();
	void BroadcastOpenNextStage();

private:
	UPROPERTY()
	TArray<TObjectPtr<AGRLevel1ControlPanel>> Panels;

	int32 EnemyCount = 0;

	void UpdateLevel1ControlPanel();

	FVector FindSpawnableLocation(AActor* AlivePlayer);
};
