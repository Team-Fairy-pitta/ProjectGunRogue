#include "Item/GRItemBoxSpawnManager.h"
#include "Item/GRItemBoxSpawnPoint.h"
#include "Item/GRItemRandomBox.h"

AGRItemBoxSpawnManager::AGRItemBoxSpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGRItemBoxSpawnManager::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SpawnItemBoxAtRandomLocation();
	}
}

void AGRItemBoxSpawnManager::SpawnItemBoxAtRandomLocation()
{
	if (SpawnPoints.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnPoints.Num() <= 0"));
		return;
	}

	if (!ItemRandomBoxClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemRandomBoxClass is INVALID"));
		return;
	}

	int32 RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
	if (!SpawnPoints.IsValidIndex(RandomIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("RandomIndex %d is InValid Index"), RandomIndex);
		return;
	}

	AGRItemBoxSpawnPoint* SpawnPoint = SpawnPoints[RandomIndex];
	if (!IsValid(SpawnPoint))
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnPoint (AGRItemBoxSpawnPoint) is INVALID"));
		return;
	}

	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("GetWorld() is INVALID"));
		return;
	}
	
	FVector TraceStart = SpawnPoint->GetActorLocation();
	FVector TraceEnd = TraceStart - FVector(0.f, 0.f, 1000.f); // 지면 방향
	FCollisionQueryParams Params;
	FHitResult Hit;

	Params.AddIgnoredActor(this);
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);

	FVector SpawnLocation = SpawnPoint->GetActorLocation();
	FRotator SpawnRotation = SpawnPoint->GetActorRotation();
	if (bIsHit)
	{
		SpawnLocation = Hit.ImpactPoint;
	}

	AGRItemRandomBox* SpwanedActor = GetWorld()->SpawnActor<AGRItemRandomBox>(ItemRandomBoxClass, SpawnLocation, SpawnRotation);
}
