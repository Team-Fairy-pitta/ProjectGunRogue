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

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is nullptr"));
		return;
	}

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();
		DropGoodsForEachPlayer(PlayerController);
	}
}

void ATestEnemyActor::DropGoodsForEachPlayer(APlayerController* Player)
{
	if (!Player)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr."));
		return;
	}

	AGRPlayerState* PlayerState = Player->GetPlayerState<AGRPlayerState>();
	if (!PlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerState is nullptr for %s"), *Player->GetName());
		return;
	}

	TArray<FDropGoodsInfo_TEST> DropInfoList = GetDropGoodsList();

	for (const FDropGoodsInfo_TEST& DropInfo : DropInfoList)
	{
		SpawnToTargetPlayer(PlayerState, DropInfo.GoodsClass, DropInfo.Count);
	}
}

TArray<FDropGoodsInfo_TEST> ATestEnemyActor::GetDropGoodsList()
{
	// NOTE: 나중에 랜덤 드랍을 구현할 수도 있음
	return DropGoodsList;
}

void ATestEnemyActor::SpawnToTargetPlayer(APlayerState* PlayerState, TSubclassOf<AGRGoodsActor> GoodsClass, int32 DropCount)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is nullptr"));
		return;
	}

	if (!GoodsClass)
	{
		UE_LOG(LogTemp, Error, TEXT("GoodsClass is nullptr"));
		return;
	}

	for (int32 i = 0; i < DropCount; ++i)
	{
		FVector RandomOffset = GetRanomOffsetAround();
		FVector XY = GetActorLocation() + RandomOffset;
		FVector SpawnLoc = GetGroundLocation(XY);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = PlayerState;

		AGRGoodsActor* GoodsActor = World->SpawnActor<AGRGoodsActor>(
			GoodsClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);

		if (!GoodsActor)
		{
			UE_LOG(LogTemp, Error, TEXT("SpawnActor failed!"));
			continue;
		}

		GoodsActor->MulticastRPC_InitGoods();
	}
}

FVector ATestEnemyActor::GetRanomOffsetAround() const
{
	FVector RandomOffset;
	RandomOffset.X = FMath::RandRange(-100.f, 100.f);
	RandomOffset.Y = FMath::RandRange(-100.f, 100.f);
	return RandomOffset;
}
