#pragma once

#include "GameFramework/Actor.h"
#include "GREnemySpawnManager.generated.h"

USTRUCT(BlueprintType)
struct FSpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	class AGREnemySpawner* Spawner = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	bool bUseFixedSpawnCount = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn", meta = (EditCondition = "bUseFixedSpawnCount", ClampMin = "0"))
	int32 FixedSpawnCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn", meta = (EditCondition = "!bUseFixedSpawnCount", ClampMin = "0"))
	FInt32Interval SpawnCountRange = { 0, 5 };
};

UCLASS()
class GUNROGUE_API AGREnemySpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AGREnemySpawnManager();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void SpawnEnemies();

	void FindAllSpawnersInWorld();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TArray<FSpawnInfo> SpawnInfos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	bool bAutoFindSpawners = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn", meta = (EditCondition = "bAutoFindSpawners", ClampMin = "0"))
	int32 AutoSpawnCount = 5;

private:
	float GetDifficultyMultiplier();
};
