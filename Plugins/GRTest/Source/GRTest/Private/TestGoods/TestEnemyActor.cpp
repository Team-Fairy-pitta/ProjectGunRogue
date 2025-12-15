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

//실제 사용 코드
FVector ATestEnemyActor::GetGroundLocation(const FVector& InXY) const
{
	if (!GetWorld())
	{
		return this->GetActorLocation();
	}

	static const FVector FallDirection = FVector(0, 0, -1.0f);
	static const float CheckDistance = 1000.0f;

	FVector Result = InXY;
	
	FVector Start = InXY + FVector(0,0, 200.f);
	FVector End = Start + FallDirection * (CheckDistance);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		Result.Z = HitResult.ImpactPoint.Z;
	}

	return Result;
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
				
				FVector XY = GetActorLocation() + RandomOffset;
				FVector SpawnLoc = GetGroundLocation(XY);
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = PS;
				
				AGRGoodsActor* Goods = World->SpawnActor<AGRGoodsActor>(
					Info.GoodsClass,
					SpawnLoc,
					FRotator::ZeroRotator,
					SpawnParams
				);

				if (!Goods)
				{
					UE_LOG(LogTemp, Error, TEXT("SpawnActor failed!"));
					continue;
				}
				
				Goods->MulticastRPC_InitGoods();
				UE_LOG(LogTemp, Warning, TEXT("Goods InitGoods Called"));
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("DropGoods() Finished"));
}


