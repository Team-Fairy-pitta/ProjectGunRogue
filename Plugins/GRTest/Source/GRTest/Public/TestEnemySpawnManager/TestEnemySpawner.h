#pragma once

#include "GameFramework/Actor.h"
#include "TestEnemySpawnRow.h"
#include "TestEnemySpawner.generated.h"

class UBoxComponent;

UCLASS()
class GRTEST_API ATestEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ATestEnemySpawner();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	AActor* SpawnRandomEnemy();


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	UDataTable* EnemyDataTable;

protected:

	FTestEnemySpawnRow* GetRandomEnemy() const;

	AActor* SpawnEnemy(TSubclassOf<AActor> EnemyClass);

	FVector GetRandomPointInVolume() const;

	FVector AdjustSpawnToGround(const FVector& StartLocation) const;

};
