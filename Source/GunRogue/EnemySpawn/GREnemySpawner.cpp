#include "GREnemySpawner.h"

#include "Components/BoxComponent.h"


AGREnemySpawner::AGREnemySpawner()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);


	EnemyDataTable = nullptr;
}

AActor* AGREnemySpawner::SpawnRandomEnemy()
{
 	if (FGREnemySpawnRow* SelectedRow = GetRandomEnemyRow())
	{
		if (UClass* ActualClass = SelectedRow->EnemyClass.Get())
		{
			return SpawnEnemy(ActualClass);
		}
	}

	return nullptr;

}

FGREnemySpawnRow* AGREnemySpawner::GetRandomEnemyRow() const
{
	if (!EnemyDataTable)
	{
		return nullptr;
	}

	TArray<FGREnemySpawnRow*> AllRows;
	static const FString ContextString(TEXT("EnemySpawnContext"));
	EnemyDataTable->GetAllRows(ContextString, AllRows);

	if (AllRows.IsEmpty())
	{
		return nullptr;
	}

	float TotalChance = 0.0f; 
	for (const FGREnemySpawnRow* Row : AllRows) 
	{
		if (Row) 
		{
			TotalChance += Row->SpawnChance; 
		}
	}


	const float RandValue = FMath::FRandRange(0.0f, TotalChance);
	float AccumulateChance = 0.0f;

	for (FGREnemySpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->SpawnChance;
		if (RandValue <= AccumulateChance)
		{
			return Row;
		}
	}


	return nullptr;
}

AActor* AGREnemySpawner::SpawnEnemy(TSubclassOf<AActor> EnemyClass)
{
	if (!EnemyClass)
	{
		return nullptr;
	}

	FVector RandomLocation = GetRandomPointInVolume();
	FVector GroundLocation = SnapSpawnPointToGround(RandomLocation);
	AActor* SpwanedActor = GetWorld()->SpawnActor<AActor>(EnemyClass, GroundLocation, FRotator::ZeroRotator);


	return SpwanedActor;
}

FVector AGREnemySpawner::GetRandomPointInVolume() const
{
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	FVector BoxOrigin = SpawningBox->GetComponentLocation();

	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		BoxExtent.Z
	);
}


FVector AGREnemySpawner::SnapSpawnPointToGround(const FVector& StartLocation) const
{
	FHitResult Hit;
	FVector Start = StartLocation + FVector(0.f, 0.f, 500.f); 
	FVector End = StartLocation - FVector(0.f, 0.f, 2000.f); 

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		return Hit.ImpactPoint; 
	}

	return StartLocation;
}
