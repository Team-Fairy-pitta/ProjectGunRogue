#pragma once

#include "GameFramework/Actor.h"
#include "TestEnemySpawnRow.h"
#include "TestEnemySpawnManager.generated.h"


UCLASS()
class GRTEST_API ATestEnemySpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ATestEnemySpawnManager();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	AActor* SpawnRandomEnemy();


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	UDataTable* EnemyDataTable;

protected:
	virtual void BeginPlay() override;

	FTestEnemySpawnRow* GetRandomEnemy() const;

	AActor* SpawnEnemy(TSubclassOf<AActor> EnemyClass, const FVector& Location);

	FVector GetRandomPointOnNavMesh() const;


};
