#include "TestEnemySpawnManager/TestEnemySpawnManager.h"

#include "TestEnemySpawnManager/TestEnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ATestEnemySpawnManager::ATestEnemySpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATestEnemySpawnManager::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoFindSpawners)
	{
		FindAllSpawners();
	}

	SpawnAllEnemies();
}

void ATestEnemySpawnManager::SpawnAllEnemies()
{
	for (const FSpawnInfo& Info : Spawners)
	{
		if (!IsValid(Info.Spawner))
		{
			continue;
		}

		for (int32 i = 0; i < Info.SpawnCount; ++i)
		{
			AActor* SpawnedEnemy = Info.Spawner->SpawnRandomEnemy();
		}
	}
	
}

void ATestEnemySpawnManager::FindAllSpawners()
{
	TArray<AActor*> FoundSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATestEnemySpawner::StaticClass(), FoundSpawners);

	TSet<ATestEnemySpawner*> ExistingSpawners;
	for (const FSpawnInfo& Info : Spawners)
	{
		if (IsValid(Info.Spawner))
		{
			ExistingSpawners.Add(Info.Spawner);
		}
	}

	for (AActor* Actor : FoundSpawners)
	{
		ATestEnemySpawner* Spawner = Cast<ATestEnemySpawner>(Actor);
		if (Spawner && !ExistingSpawners.Contains(Spawner))
		{
			FSpawnInfo Info;
			Info.Spawner = Spawner;
			Info.SpawnCount = 5; 
			Spawners.Add(Info);
		}
	}
}





