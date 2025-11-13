#pragma once

#include "GameFramework/Actor.h"
#include "TestEnemySpawnRow.h"
#include "TestEnemySpawnManager.generated.h"

class UBoxComponent;

UCLASS()
class GRTEST_API ATestEnemySpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ATestEnemySpawnManager();

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
	virtual void BeginPlay() override;

	FTestEnemySpawnRow* GetRandomEnemy() const;

	AActor* SpawnEnemy(TSubclassOf<AActor> EnemyClass);

	FVector GetRandomPointInVolume() const;

	FVector AdjustSpawnToGround(const FVector& StartLocation) const;

};
