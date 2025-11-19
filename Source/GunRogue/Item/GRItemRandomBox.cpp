#include "Item/GRItemRandomBox.h"
#include "Item/GRItemDefinition.h"
#include "Item/GRItemInfoTableRow.h"
#include "Character/GRCharacter.h"
#include "Player/GRPlayerState.h"
#include "Components/StaticMeshComponent.h"

AGRItemRandomBox::AGRItemRandomBox()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneRoot);
}

void AGRItemRandomBox::BeginPlay()
{
	Super::BeginPlay();
}

TArray<TObjectPtr<UStaticMeshComponent>> AGRItemRandomBox::GetMeshComponents()
{
	TArray<TObjectPtr<UStaticMeshComponent>> MeshComponents;
	MeshComponents.Add(StaticMeshComponent);
	return MeshComponents;
}

void AGRItemRandomBox::InteractWith(AActor* OtherActor)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("InteractWith() REQUIRES authority"));
		return;
	}

	AGRCharacter* GRCharacter = Cast<AGRCharacter>(OtherActor);
	if (!IsValid(GRCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("GRCharacter (AGRCharacter) is INVALID"));
		return;
	}

	AGRPlayerState* GRPlayerState = GRCharacter->GetPlayerState<AGRPlayerState>();
	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerState (AGRPlayerState) is INVALID"));
		return;
	}

	if (WasActivatedSet.Contains(GRPlayerState))
	{
		UE_LOG(LogTemp, Display, TEXT("%s has activated this item box..."), *GRPlayerState->GetPlayerName());
		return;
	}

	TArray<UGRItemDefinition*> ItemDefinitions = GetNewRandomItems(GRPlayerState);

	SpawnItemsToSpecificPlayer(GRPlayerState, ItemDefinitions);

	WasActivatedSet.Add(GRPlayerState);
}

void AGRItemRandomBox::OnOver()
{
}

void AGRItemRandomBox::OnOut()
{
}

bool AGRItemRandomBox::CanInteract(AActor* OtherActor)
{
	if (OtherActor->IsA<AGRCharacter>())
	{
		AGRCharacter* GRCharacter = Cast<AGRCharacter>(OtherActor);
		if (IsValid(GRCharacter))
		{
			AGRPlayerState* GRPlayerState = GRCharacter->GetGRPlayerState();
			if (IsValid(GRPlayerState))
			{
				bool bWasActivate = WasActivatedSet.Contains(GRPlayerState);
				return !bWasActivate;
			}
		}
	}
	else if (OtherActor->IsA<AGRPlayerState>())
	{
		AGRPlayerState* GRPlayerState = Cast<AGRPlayerState>(OtherActor);
		if (IsValid(GRPlayerState))
		{
			bool bWasActivate = WasActivatedSet.Contains(GRPlayerState);
			return !bWasActivate;
		}
	}
	return false;
}

TArray<UGRItemDefinition*> AGRItemRandomBox::GetNewRandomItems(AGRPlayerState* GRPlayerState)
{
	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerState (AGRPlayerState) is INVALID"));
		return TArray<UGRItemDefinition*>();
	}

	TArray<UGRItemDefinition*> Items;

	// 최대 3개 까지, 
	// 내가(GRPlayerState 확인) 가지고 있지 않은 아이템 중에서,
	// 겹치지 않는(Items 확인) 아이템 Definition을 랜덤 선택함
	while (Items.Num() < 3)
	{
		UGRItemDefinition* RandomItem = GetNewRandomItem(GRPlayerState, Items);
		if (!RandomItem)
		{
			break;
		}
		else
		{
			Items.Add(RandomItem);
		}
	}

	return Items;
}

UGRItemDefinition* AGRItemRandomBox::GetNewRandomItem(AGRPlayerState* GRPlayerState, TArray<UGRItemDefinition*>& AlreadySelected)
{
	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerState (AGRPlayerState) is INVALID"));
		return nullptr;
	}

	if (!ItemTable)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemTable (UDataTable) is INVALID"));
		return nullptr;
	}

	TArray<FGRItemInfoTableRow*> AllRows;
	static const FString ContextString(TEXT("AGRItemRandomBox::GetRandomItem"));
	ItemTable->GetAllRows(ContextString, AllRows);

	float RarityProbatilty_Sum = RarityProbatility_Normal;
	RarityProbatilty_Sum += RarityProbatility_Rare;
	RarityProbatilty_Sum += RarityProbatility_Epic;

	// 레어리티 랜덤 선택 로직
	// 숫자가 클 수록, 그 레어리티가 나올 확률이 높다는 뜻
	// 확률 공식은 A / (A + B + C)
	float RarityDice = FMath::FRandRange(0.0f, RarityProbatilty_Sum);
	EItemRarity SelectedRarity = EItemRarity::NONE;
	if (RarityDice < RarityProbatility_Normal)
	{
		SelectedRarity = EItemRarity::NORMAL;
	}
	else
	{
		RarityDice -= RarityProbatility_Normal;
		if (RarityDice < RarityProbatility_Rare)
		{
			SelectedRarity = EItemRarity::RARE;
		}
		else
		{
			SelectedRarity = EItemRarity::EPIC;
		}
	}

	TArray<UGRItemDefinition*> Targets;
	for (const FGRItemInfoTableRow* Row : AllRows)
	{
		if (Row)
		{
			if (Row->Rarity == SelectedRarity)
			{
				if (!GRPlayerState->HasItem(Row->ItemDefinition))
				{
					if (!AlreadySelected.Contains(Row->ItemDefinition))
					{
						Targets.Add(Row->ItemDefinition);
					}
				}
			}
		}
	}

	if (Targets.Num() <= 0)
	{
		return nullptr;
	}
	else
	{
		int32 RandomIndex = FMath::RandRange(0, Targets.Num() - 1);
		return Targets[RandomIndex];
	}
}

void AGRItemRandomBox::SpawnItemsToSpecificPlayer(AGRPlayerState* GRPlayerState, TArray<UGRItemDefinition*> ItemDefinitions)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnItemsToSpecificPlayer() REQUIRES authority"));
		return;
	}

	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerState (AGRPlayerState) is INVALID"));
		return;
	}

	static TArray<FVector> SpawnLocations =
	{
		FVector(0, 0, 100),
		FVector(-100, 0, 100),
		FVector(100, 0, 100)
	};

	SpawnedActors.Empty();
	for (int32 ItemIndex = 0; ItemIndex < ItemDefinitions.Num(); ++ItemIndex)
	{
		UGRItemDefinition* ItemDefinition = ItemDefinitions[ItemIndex];
		if (ItemDefinition)
		{
			FVector SpawnLocation = GetActorLocation();
			SpawnLocation += SpawnLocations[ItemIndex].X * GetActorForwardVector();
			SpawnLocation += SpawnLocations[ItemIndex].Y * GetActorRightVector();
			SpawnLocation += SpawnLocations[ItemIndex].Z * GetActorUpVector();
			SpawnItemToSpecificPlayer(GRPlayerState, ItemDefinition, SpawnLocation);
		}
	}
}

void AGRItemRandomBox::SpawnItemToSpecificPlayer(AGRPlayerState* GRPlayerState, UGRItemDefinition* ItemDefinition, FVector& Location)
{
	// 일단 스폰
	// TODO: 특정 캐릭터에 대해서만 스폰
	// TODO: 캐릭터마다 Interact With 정보 따로 저장해서 ,각각 사용할 수 있도록 하기

	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnItemToSpecificPlayer() REQUIRES authority"));
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("GetWorld() is INVALID"));
		return;
	}

	FRotator SpawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = GRPlayerState;

	AGRItemActor* ItemActor = World->SpawnActor<AGRItemActor>(AGRItemActor::StaticClass(), Location, SpawnRotation, SpawnParam);
	if (ItemActor)
	{
		ItemActor->OnPickup.AddUObject(this, &ThisClass::OnPickupAnyItem);
		ItemActor->MulticastRPC_InitItem(ItemDefinition);
		SpawnedActors.Add(ItemActor);
	}
}

void AGRItemRandomBox::OnPickupAnyItem()
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("OnPickupAnyItem() REQUIRES authority"));
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("OnPickupAnyItem()"));

	// 아이템 하나를 집으면, 다른 아이템은 전부 제거해야 함
	for (TWeakObjectPtr<AActor> WeakActor : SpawnedActors)
	{
		if (WeakActor.IsValid())
		{
			WeakActor.Get()->Destroy();
		}
	}
}
