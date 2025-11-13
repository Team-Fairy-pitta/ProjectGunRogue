#include "TestEnemySpawnManager/TestEnemySpawnGameMode.h"

#include "TestEnemySpawnManager/TestEnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

void ATestEnemySpawnGameMode::BeginPlay()
{
	Super::BeginPlay();

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys)
	{
		NavSys->Build();
	}

}

void ATestEnemySpawnGameMode::Spawn()
{
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATestEnemySpawner::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = 50;

	for (int32 i = 0; i < ItemToSpawn; ++i)
	{
		if (FoundVolumes.Num() > 0)
		{
			ATestEnemySpawner* SpawnVolume = Cast<ATestEnemySpawner>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomEnemy();
			}
		}
	}
}


