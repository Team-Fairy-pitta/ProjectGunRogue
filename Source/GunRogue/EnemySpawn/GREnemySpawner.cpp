#include "GREnemySpawner.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"


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

	// 콜리전 높이 보정 
	float HalfHeight = 0.f;
	if (ACharacter* DefaultChar = Cast<ACharacter>(EnemyClass->GetDefaultObject()))
	{
		if (UCapsuleComponent* Capsule = DefaultChar->GetCapsuleComponent())
		{
			HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
		}
	}
	GroundLocation.Z += HalfHeight;

	// 스폰 충돌 시 가능한 범위 내에서 위치를 조정
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;


	AActor* SpwanedActor = GetWorld()->SpawnActor<AActor>(EnemyClass, GroundLocation, FRotator::ZeroRotator, Params);


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
	FVector Start = StartLocation; 
	FVector End = StartLocation - FVector(0.f, 0.f, 2000.f); 

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		return Hit.ImpactPoint; 
	}

	return StartLocation;
}
