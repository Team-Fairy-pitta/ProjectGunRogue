// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Character/GRAICharacter.h"
#include "AI/Controller/GRAIController.h"
#include "Player/GRPlayerState.h"
#include "Goods/GRGoodsActor.h"
#include "Character/GRZLocationComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "GameModes/Level1/GRGameMode_Level1.h"
#include "AbilitySystemComponent.h"
#include "BrainComponent.h"
#include "Windows/WindowsApplication.h"

AGRAICharacter::AGRAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->bUseControllerDesiredRotation = true;
		MoveComp->bOrientRotationToMovement = false;

		MoveComp->bUseRVOAvoidance = true;
		MoveComp->AvoidanceConsiderationRadius = 100.f;
		MoveComp->AvoidanceWeight = 0.5f;

		MoveComp->bRequestedMoveUseAcceleration = true;
	}
	bUseControllerRotationYaw = false;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	SetNetUpdateFrequency(100.0f);

	ZLocationComponent = CreateDefaultSubobject<UGRZLocationComponent>(TEXT("ZLocationComponent"));

	HealthAttributeSet = CreateDefaultSubobject<UGRHealthAttributeSet>(TEXT("HealthAttributeSet"));
	CombatAttributeSet = CreateDefaultSubobject<UGRCombatAttributeSet>(TEXT("CombatAttributeSet"));

	USkeletalMeshComponent* SkelMesh = GetMesh();
	if (SkelMesh)
	{
		SkelMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	}
}

UAbilitySystemComponent* AGRAICharacter::GetAbilitySystemComponent() const
{
	return ASC;
}


void AGRAICharacter::BeginPlay()
{
	Super::BeginPlay();

	InitAbilitySystemComponent();
	InitStatus();
	NotifySpawnToGameMode();
}

void AGRAICharacter::EndPlay(EEndPlayReason::Type EndPlayReapon)
{
	Super::EndPlay(EndPlayReapon);

	NotifyDestroyToGameMode();
}

void AGRAICharacter::InitAbilitySystemComponent()
{
	if (HasAuthority())
	{
		if (ASC)
		{
			ASC->InitAbilityActorInfo(this, this);

			for (auto& AbilityClass : AttackAbilityClassList)
			{
				if (AbilityClass)
				{
					FGameplayAbilitySpec Spec(AbilityClass, /*Level*/1, /*InputID*/0, this);
					ASC->GiveAbility(Spec);
				}
			}

			ASC->GetGameplayAttributeValueChangeDelegate(UGRHealthAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
		}
	}
}

void AGRAICharacter::InitStatus()
{
	if (ASC)
	{
		ASC->SetNumericAttributeBase(UGRHealthAttributeSet::GetMaxHealthAttribute(), InitMaxHealth);
		ASC->SetNumericAttributeBase(UGRHealthAttributeSet::GetHealthAttribute(), InitMaxHealth);

		ASC->SetNumericAttributeBase(UGRHealthAttributeSet::GetMaxShieldAttribute(), InitMaxShield);
		ASC->SetNumericAttributeBase(UGRHealthAttributeSet::GetShieldAttribute(), InitMaxShield);
	}
}

void AGRAICharacter::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;
	if (Health <= 0 && bIsDead == false)
	{
		OnDead();
	}
}

void AGRAICharacter::OnDead()
{
	bIsDead = true;

	DropGoods();
	MulticastRPC_OnDead();
}

void AGRAICharacter::MulticastRPC_OnDead_Implementation()
{
	if (HasAuthority())
	{
		OnDead_ProcessAuth();
	}
	OnDead_ProcessNormal();
}

void AGRAICharacter::OnDead_ProcessAuth()
{
	float BodyLifeSpan = 1.0f;
	SetLifeSpan(BodyLifeSpan);

	AGRAIController* AIController = GetController<AGRAIController>();
	if (IsValid(AIController) && AIController->BrainComponent)
	{
		AIController->BrainComponent->Cleanup();
	}
}

void AGRAICharacter::OnDead_ProcessNormal()
{
	USkeletalMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent)
	{
		MeshComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
		MeshComponent->SetSimulatePhysics(true);
		MeshComponent->SetAllBodiesPhysicsBlendWeight(1.0f);
		MeshComponent->SetCollisionProfileName(FName(TEXT("Ragdoll")));
		MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		MeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	}

	if (ASC)
	{
		FGameplayTag ImpactCueTag = FGameplayTag::RequestGameplayTag("GameplayCue.AI.Explosion");
		FGameplayCueParameters Params;
		Params.Location = GetActorLocation();

		ASC->ExecuteGameplayCue(ImpactCueTag, Params);
	}
}

void AGRAICharacter::NotifySpawnToGameMode()
{
	if (!HasAuthority())
	{
		return;
	}
	if (!GetWorld())
	{
		return;
	}

	AGRGameMode_Level1* Level1GameMode = GetWorld()->GetAuthGameMode<AGRGameMode_Level1>();
	if (!IsValid(Level1GameMode))
	{
		UE_LOG(LogTemp, Error, TEXT("InteractWidgetClass is INVALID"));
		return;
	}

	Level1GameMode->ReceiveSpawnEnemy();
}

void AGRAICharacter::NotifyDestroyToGameMode()
{
	if (!HasAuthority())
	{
		return;
	}
	if (!GetWorld())
	{
		return;
	}

	AGRGameMode_Level1* Level1GameMode = GetWorld()->GetAuthGameMode<AGRGameMode_Level1>();
	if (!IsValid(Level1GameMode))
	{
		UE_LOG(LogTemp, Error, TEXT("InteractWidgetClass is INVALID"));
		return;
	}

	Level1GameMode->ReceiveDestroyEnemy();
}


#pragma region Drops
void AGRAICharacter::DropGoods()
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

void AGRAICharacter::DropGoodsForEachPlayer(APlayerController* Player)
{
	if (!Player)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr."));
		return;
	}

	AGRPlayerState* GRPlayerState = Player->GetPlayerState<AGRPlayerState>();
	if (!GRPlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerState is nullptr for %s"), *Player->GetName());
		return;
	}

	TArray<FDropGoodsInfo> DropInfoList = GetDropGoodsList();

	for (const FDropGoodsInfo& DropInfo : DropInfoList)
	{
		if (!CanDropGoods(DropInfo.DropChance))
		{
			continue;
		}
		
		int32 RandomCount = GetDropCount(DropInfo.Count, DropInfo.RandomRange);
		
		SpawnToTargetPlayer(GRPlayerState, DropInfo.GoodsClass, RandomCount);
	}
}

bool AGRAICharacter::CanDropGoods(float Chance) const
{
	return FMath::FRand() <= Chance;
}

int32 AGRAICharacter::GetDropCount(int32 BaseCount, int32 RandomRange) const
{
	const int32 MinCount = FMath::Max(1, BaseCount - RandomRange);
	const int32 MaxCount = BaseCount + RandomRange;

	return FMath::RandRange(MinCount, MaxCount);
}


TArray<FDropGoodsInfo> AGRAICharacter::GetDropGoodsList()
{
	return DropGoodsList;
}

void AGRAICharacter::SpawnToTargetPlayer(APlayerState* InPlayerState, TSubclassOf<AGRGoodsActor> GoodsClass, int32 DropCount)
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
		FVector RandomOffset = GetRandomOffsetAround();
		FVector XY = GetActorLocation() + RandomOffset;
		FVector SpawnLoc = GetGroundLocation(XY);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = InPlayerState;

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

FVector AGRAICharacter::GetRandomOffsetAround() const
{
	FVector RandomOffset;
	RandomOffset.X = FMath::RandRange(-100.f, 100.f);
	RandomOffset.Y = FMath::RandRange(-100.f, 100.f);
	return RandomOffset;
}

FVector AGRAICharacter::GetGroundLocation(const FVector& InXY) const
{
	if (!GetWorld())
	{
		return this->GetActorLocation();
	}

	static const FVector FallDirection = FVector(0, 0, -1.0f);
	static const float CheckDistance = 1000.0f;

	FVector Result = InXY;

	FVector Start = InXY + FVector(0, 0,  0);
	FVector End = Start + FallDirection * (CheckDistance);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	if (GetWorld()->LineTraceSingleByObjectType(HitResult, Start, End, ECC_WorldStatic, Params))
	{
		Result.Z = HitResult.ImpactPoint.Z;
	}

	return Result;
}

#pragma endregion
