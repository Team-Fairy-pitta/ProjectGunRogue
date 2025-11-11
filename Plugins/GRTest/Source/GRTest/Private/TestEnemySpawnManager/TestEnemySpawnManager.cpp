#include "TestEnemySpawnManager/TestEnemySpawnManager.h"

#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "AI/Navigation/NavigationTypes.h"



ATestEnemySpawnManager::ATestEnemySpawnManager()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	EnemyDataTable = nullptr;
}

AActor* ATestEnemySpawnManager::SpawnRandomEnemy()
{
	if (FTestEnemySpawnRow* SelectedRow = GetRandomEnemy())
	{
		if (UClass* ActualClass = SelectedRow->EnemyClass.Get())
		{
			FVector SpawnLocation = GetRandomPointOnNavMesh();
			return SpawnEnemy(ActualClass, SpawnLocation);
		}
	}

	return nullptr;

}

void ATestEnemySpawnManager::BeginPlay()
{
	Super::BeginPlay();
	
}

FTestEnemySpawnRow* ATestEnemySpawnManager::GetRandomEnemy() const
{
	if (!EnemyDataTable)
	{
		return nullptr;
	}

	TArray<FTestEnemySpawnRow*> AllRows;
	static const FString ContextString(TEXT("EnemySpawnContext"));
	EnemyDataTable->GetAllRows(ContextString, AllRows);

	if (AllRows.IsEmpty())
	{
		return nullptr;
	}

	float TotalChance = 0.0f; 
	for (const FTestEnemySpawnRow* Row : AllRows) 
	{
		if (Row) 
		{
			TotalChance += Row->SpawnChance; 
		}
	}


	const float RandValue = FMath::FRandRange(0.0f, TotalChance);
	float AccumulateChance = 0.0f;

	for (FTestEnemySpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->SpawnChance;
		if (RandValue <= AccumulateChance)
		{
			return Row;
		}
	}


	return nullptr;
}

AActor* ATestEnemySpawnManager::SpawnEnemy(TSubclassOf<AActor> EnemyClass, const FVector& Location)
{
	if (!EnemyClass)
	{
		return nullptr;
	}

	FRotator SpawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	return GetWorld()->SpawnActor<AActor>(EnemyClass, Location, SpawnRotation, Params);

}

FVector ATestEnemySpawnManager::GetRandomPointOnNavMesh() const
{
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSystem)
	{
		return FVector::ZeroVector;
	}

	FNavLocation RandomPoint;

	const FVector Origin = GetActorLocation();
	const float SearchRadius = 50000.f; 

	if (NavSystem->GetRandomReachablePointInRadius(Origin, SearchRadius, RandomPoint))
	{
		return RandomPoint.Location;
	}

	if (NavSystem->GetRandomPoint(RandomPoint))
	{
		return RandomPoint.Location;
	}

	return FVector::ZeroVector;

}

