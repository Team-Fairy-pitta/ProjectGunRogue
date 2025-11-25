#include "Item/GRItemRandomBox.h"
#include "Item/GRItemDefinition.h"
#include "Item/GRItemInfoTableRow.h"
#include "Character/GRCharacter.h"
#include "Player/GRPlayerState.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

AGRItemRandomBox::AGRItemRandomBox()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	BoxBottom = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxBottom"));
	BoxBottom->SetupAttachment(SceneRoot);

	BoxLid = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxLid"));
	BoxLid->SetupAttachment(SceneRoot);
}

void AGRItemRandomBox::BeginPlay()
{
	Super::BeginPlay();
}

void AGRItemRandomBox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, WasActivatedArray);
}

TArray<TObjectPtr<UStaticMeshComponent>> AGRItemRandomBox::GetMeshComponents()
{
	TArray<TObjectPtr<UStaticMeshComponent>> MeshComponents;
	MeshComponents.Add(BoxBottom);
	MeshComponents.Add(BoxLid);
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

	if (WasActivatedArray.Contains(GRPlayerState))
	{
		UE_LOG(LogTemp, Display, TEXT("%s has activated this item box..."), *GRPlayerState->GetPlayerName());
		return;
	}

	// 'GRPlayerState'가 이 아이템 박스를 사용하지 못하도록 한다. (1회용 상호 작용)
	DisableToSpecificPlayer(GRPlayerState);

	// 박스 Open 애니메이션 재생
	// 애니메이션 재생이 끝나면 처리 시작
	MulticastRPC_StartAnimation(GRPlayerState);
}

void AGRItemRandomBox::OnOver()
{
}

void AGRItemRandomBox::OnOut()
{
}

bool AGRItemRandomBox::CanInteract(AActor* OtherActor)
{
	// 사용한 적이 없는 Player만 이 아이템 박스와 상호작용 할 수 있다.
	// 'WasActivatedArray' 배열을 이용해 사용한 적이 있는지 없는지 판단한다.

	if (OtherActor->IsA<AGRCharacter>())
	{
		AGRCharacter* GRCharacter = Cast<AGRCharacter>(OtherActor);
		if (IsValid(GRCharacter))
		{
			AGRPlayerState* GRPlayerState = GRCharacter->GetGRPlayerState();
			if (IsValid(GRPlayerState))
			{
				bool bWasActivate = WasActivatedArray.Contains(GRPlayerState);
				return !bWasActivate;
			}
		}
	}
	else if (OtherActor->IsA<AGRPlayerState>())
	{
		AGRPlayerState* GRPlayerState = Cast<AGRPlayerState>(OtherActor);
		if (IsValid(GRPlayerState))
		{
			bool bWasActivate = WasActivatedArray.Contains(GRPlayerState);
			return !bWasActivate;
		}
	}
	return false;
}

void AGRItemRandomBox::MulticastRPC_StartAnimation_Implementation(AGRPlayerState* GRPlayerState)
{
	StartOpenAnimation(GRPlayerState);
}

void AGRItemRandomBox::OnFinishOpenAnimation(AGRPlayerState* GRPlayerState)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("InteractWith() REQUIRES authority"));
		return;
	}

	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerState (AGRPlayerState) is INVALID"));
		return;
	}

	// 'GRPlayerState'가 가지고 있지 않은 아이템을 랜덤으로 여러 개 선택한 다음,
	// 'GRPlayerState'의 게임 화면에만 아이템을 스폰한다.
	TArray<UGRItemDefinition*> ItemDefinitions = GetNewRandomItems(GRPlayerState);
	SpawnItemsToSpecificPlayer(GRPlayerState, ItemDefinitions);
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
		FVector(0, -100, 100),
		FVector(0, 100, 100)
	};

	SpawnedActors.Add(GRPlayerState);

	int32 Max = ItemDefinitions.Num() <= 3 ? ItemDefinitions.Num() : 3;
	for (int32 ItemIndex = 0; ItemIndex < Max; ++ItemIndex)
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

	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerState (AGRPlayerState) is INVALID"));
		return;
	}

	FRotator SpawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = GRPlayerState; /* 특정 플레이어에게만 아이템을 보여줌 */

	AGRItemActor* ItemActor = World->SpawnActor<AGRItemActor>(AGRItemActor::StaticClass(), Location, SpawnRotation, SpawnParam);
	if (ItemActor)
	{
		ItemActor->OnPickup.AddUObject(this, &ThisClass::OnPickupAnyItem);
		ItemActor->MulticastRPC_InitItem(ItemDefinition);
		SpawnedActors[GRPlayerState].Actors.Add(ItemActor);
	}
}

void AGRItemRandomBox::DisableToSpecificPlayer(AGRPlayerState* GRPlayerState)
{
	WasActivatedArray.Add(GRPlayerState);
}

void AGRItemRandomBox::OnPickupAnyItem(AGRPlayerState* GRPlayerState)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("OnPickupAnyItem() REQUIRES authority"));
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("OnPickupAnyItem()"));

	// 아이템 하나를 집으면, 다른 아이템은 전부 제거해야 함
	if (!SpawnedActors.Contains(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnedActors: Key GRPlayerState is INVALID"));
		return;
	}

	for (TWeakObjectPtr<AActor> WeakActor : SpawnedActors[GRPlayerState].Actors)
	{
		if (WeakActor.IsValid())
		{
			WeakActor.Get()->Destroy();
		}
	}
}
