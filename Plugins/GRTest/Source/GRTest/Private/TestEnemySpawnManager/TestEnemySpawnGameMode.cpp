#include "TestEnemySpawnManager/TestEnemySpawnGameMode.h"

#include "TestEnemySpawnManager/TestEnemySpawnManager.h"
#include <Kismet/GameplayStatics.h>

void ATestEnemySpawnGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATestEnemySpawnManager::StaticClass(), FoundVolumes);


	const int32 ItemToSpawn = 50;

	for (int32 i = 0; i < ItemToSpawn; ++i)
	{
		if (FoundVolumes.Num() > 0)
		{
			ATestEnemySpawnManager* SpawnVolume = Cast<ATestEnemySpawnManager>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomEnemy();
			}
		}
	}
}
