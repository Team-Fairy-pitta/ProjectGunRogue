#include "GREnemySpawnManager.h"
#include "GREnemySpawner.h"
#include "GameModes/Level1/GRGameState_Level1.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AGREnemySpawnManager::AGREnemySpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGREnemySpawnManager::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (bAutoFindSpawners)
		{
			FindAllSpawnersInWorld();
		}

		SpawnEnemies();
	}
}

float AGREnemySpawnManager::GetDifficultyMultiplier()
{
	if (!GetWorld())
	{
		return 1.0f;
	}

	AGRGameState_Level1* GRGameState = GetWorld()->GetGameState<AGRGameState_Level1>();
	if (!IsValid(GRGameState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRGameState is INVALID"));
		return 1.0f;
	}

	if (!GRGameState->GetCurrentNodeInfo())
	{
		return 1.0f;
	}
	ENodeType CurrentNodeType = GRGameState->GetCurrentNodeInfo()->NodeType;
	if (CurrentNodeType == ENodeType::HARD)
	{
		return 1.5f;
	}
	else
	{
		return 1.0f;
	}
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

		SpawnCount = (int32)((float)SpawnCount * GetDifficultyMultiplier());

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





