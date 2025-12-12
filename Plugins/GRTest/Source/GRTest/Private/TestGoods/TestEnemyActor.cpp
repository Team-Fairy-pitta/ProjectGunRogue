// Fill out your copyright notice in the Description page of Project Settings.


#include "TestGoods/TestEnemyActor.h"

#include "Components/SphereComponent.h"
#include "Goods/GRGoodsActor.h"
#include "Kismet/GameplayStatics.h"
#include "Player/GRPlayerState.h"
#include "Character/GRCharacter.h"


ATestEnemyActor::ATestEnemyActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneRoot);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(500.f);
	SphereComponent->SetupAttachment(SceneRoot);

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ATestEnemyActor::OnBeginOverlap);
}

void ATestEnemyActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATestEnemyActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->IsA<AGRCharacter>())
	{
		return;
	}

	DropGoods();

	Destroy();
}

//실제 사용할 코드
void ATestEnemyActor::DropGoods()
{
	if (!HasAuthority())
	{
		return; 
	}
	
	UE_LOG(LogTemp, Warning, TEXT("DropGoods() Called"));

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is nullptr"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("World Found. Iterating PlayerControllers..."));

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC)
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr. Skipping."));
			continue;
		}

		UE_LOG(LogTemp, Warning, TEXT("Found PlayerController: %s"), *PC->GetName());

		AGRPlayerState* PS = PC->GetPlayerState<AGRPlayerState>();
		if (!PS)
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerState is nullptr for %s"), *PC->GetName());
			continue;
		}

		UE_LOG(LogTemp, Warning, TEXT("PlayerState Found: %s"), *PS->GetName());

		for (const FDropGoodsInfo& Info : DropGoodsList)
		{
			UE_LOG(LogTemp, Warning, TEXT("Processing DropGoodsInfo"));

			if (!Info.GoodsClass)
			{
				UE_LOG(LogTemp, Error, TEXT("GoodsClass is nullptr"));
				continue;
			}
			
			for (int32 i = 0; i < Info.Count; ++i)
			{
				UE_LOG(LogTemp, Warning, TEXT("Spawning Goods [%d/%d]"), i + 1, Info.Count);

				FVector RandomOffset;
				RandomOffset.X = FMath::RandRange(-100.f, 100.f);
				RandomOffset.Y = FMath::RandRange(-100.f, 100.f);
				
				FVector SpawnLoc = GetActorLocation() + RandomOffset;
				FTransform SpawnTransform(SpawnLoc);
				
				AGRGoodsActor* Goods = World->SpawnActorDeferred<AGRGoodsActor>(
					Info.GoodsClass,
					SpawnTransform,
					nullptr,
					nullptr,
					ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
				);

				if (!Goods)
				{
					UE_LOG(LogTemp, Error, TEXT("SpawnActorDeferred failed!"));
					continue;
				}

				UE_LOG(LogTemp, Warning, TEXT("Goods Spawned (Deferred): %s"), *Goods->GetName());

				Goods->SetOwner(PS);
				UE_LOG(LogTemp, Warning, TEXT("Goods Owner Set: %s"), *PS->GetName());

				Goods->MulticastRPC_InitGoods();
				UE_LOG(LogTemp, Warning, TEXT("Goods InitGoods Called"));

				UGameplayStatics::FinishSpawningActor(Goods, SpawnTransform);
				UE_LOG(LogTemp, Warning, TEXT("FinishSpawningActor Completed: %s"), *Goods->GetName());
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("DropGoods() Finished"));
}


