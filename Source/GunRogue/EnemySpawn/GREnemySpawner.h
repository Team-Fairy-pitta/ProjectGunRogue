#pragma once

#include "GameFramework/Actor.h"
#include "GREnemySpawnRow.h"
#include "GREnemySpawner.generated.h"

class UBoxComponent;

UCLASS()
class GUNROGUE_API AGREnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AGREnemySpawner();

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

	FGREnemySpawnRow* GetRandomEnemy() const;

	AActor* SpawnEnemy(TSubclassOf<AActor> EnemyClass);

	FVector GetRandomPointInVolume() const;

	FVector AdjustSpawnToGround(const FVector& StartLocation) const;

};
