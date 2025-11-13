#pragma once

#include "GameFramework/Actor.h"
#include "TestEnemySpawnManager.generated.h"

USTRUCT(BlueprintType)
struct FSpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	class ATestEnemySpawner* Spawner = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	int32 SpawnCount = 1;
};

UCLASS()
class GRTEST_API ATestEnemySpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ATestEnemySpawnManager();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void SpawnAllEnemies();

	void FindAllSpawners();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TArray<FSpawnInfo> Spawners;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	bool bAutoFindSpawners = true;

};
