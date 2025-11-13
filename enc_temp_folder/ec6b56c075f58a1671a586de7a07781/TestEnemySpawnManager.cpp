#include "TestEnemySpawnManager/TestEnemySpawnManager.h"

#include "Components/BoxComponent.h"




ATestEnemySpawnManager::ATestEnemySpawnManager()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);


	EnemyDataTable = nullptr;
}

AActor* ATestEnemySpawnManager::SpawnRandomEnemy()
{
	if (FTestEnemySpawnRow* SelectedRow = GetRandomEnemy())
	{
		if (UClass* ActualClass = SelectedRow->EnemyClass.Get())
		{
			return SpawnEnemy(ActualClass);

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

AActor* ATestEnemySpawnManager::SpawnEnemy(TSubclassOf<AActor> EnemyClass)
{
	if (!EnemyClass)
	{
		return nullptr;
	}

	AActor* SpwanedActor = GetWorld()->SpawnActor<AActor>(
		EnemyClass,
		AdjustSpawnToGround(GetRandomPointInVolume()),
		FRotator::ZeroRotator
	);

	return SpwanedActor;


}

FVector ATestEnemySpawnManager::GetRandomPointInVolume() const
{
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	FVector BoxOrigin = SpawningBox->GetComponentLocation();

	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		BoxExtent.Z
	);
}


FVector ATestEnemySpawnManager::AdjustSpawnToGround(const FVector& StartLocation) const
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
