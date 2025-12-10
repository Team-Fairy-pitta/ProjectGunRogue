#include "GREnemySpawnManager.h"

#include "GREnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AGREnemySpawnManager::AGREnemySpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGREnemySpawnManager::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoFindSpawners)
	{
		FindAllSpawnersInWorld();
	}

	SpawnEnemies();
}

void AGREnemySpawnManager::SpawnEnemies()
{
	for (const FSpawnInfo& Info : SpawnInfos)
	{
		if (!IsValid(Info.Spawner))
		{
			continue;
		}

		int32 SpawnCount = 0;
		if (Info.bUseFixedSpawnCount)
		{
			SpawnCount = Info.FixedSpawnCount;
		}
		else
		{
			SpawnCount = FMath::RandRange(Info.SpawnCountRange.Min, Info.SpawnCountRange.Max);			
		}

		for (int32 i = 0; i < SpawnCount; ++i)
		{
			AActor* SpawnedEnemy = Info.Spawner->SpawnRandomEnemy();
		}
		
	}
	
}

void AGREnemySpawnManager::FindAllSpawnersInWorld()
{
	TArray<AActor*> FoundSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGREnemySpawner::StaticClass(), FoundSpawners);

	TSet<AGREnemySpawner*> ExistingSpawners;
	for (const FSpawnInfo& Info : SpawnInfos)
	{
		if (IsValid(Info.Spawner))
		{
			ExistingSpawners.Add(Info.Spawner);
		}
	}

	for (AActor* Actor : FoundSpawners)
	{
		AGREnemySpawner* Spawner = Cast<AGREnemySpawner>(Actor);
		if (Spawner && !ExistingSpawners.Contains(Spawner))
		{
			FSpawnInfo Info;
			Info.Spawner = Spawner;
			Info.bUseFixedSpawnCount = true;
			Info.FixedSpawnCount = AutoSpawnCount;
			SpawnInfos.Add(Info);
		}
	}
}





