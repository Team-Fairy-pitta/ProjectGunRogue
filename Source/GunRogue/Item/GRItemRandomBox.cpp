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

	TArray<UGRItemDefinition*> ItemDefinitions = GetNewRandomItems(GRPlayerState);

	for (auto* a : ItemDefinitions)
	{
		FString Message = a->ItemName.ToString();
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, Message);
	}
}

void AGRItemRandomBox::OnOver()
{
}

void AGRItemRandomBox::OnOut()
{
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
