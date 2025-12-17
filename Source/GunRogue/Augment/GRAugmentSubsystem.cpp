// GRAugmentSubsystem.cpp


#include "Augment/GRAugmentSubsystem.h"

#include "GRAugmentDefinition.h"
#include "Engine/DataTable.h"
#include "GRAugmentInfoRow.h"
#include "Player/GRPlayerState.h"

void UGRAugmentSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AugmentDataTablePath = FSoftObjectPath("/Game/GunRogue/Blueprints/Augment/GRAugmentInfoRow.GRAugmentInfoRow");

	if (AugmentDataTablePath.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("UGRAugmentSubsystem: AugmentSubsystem DataTable path is null"));
		return;
	}

	UDataTable* Table = Cast<UDataTable>(AugmentDataTablePath.TryLoad());
	if (!Table)
	{
		return;
	}

	TArray<FGRAugmentInfoRow*> Rows;
	Table->GetAllRows(TEXT("AugmentInit"), Rows);

	for (auto* Row : Rows)
	{
		if (Row->AugmentDefinition)
		{
			AugmentMap.Add(Row->AugmentID, Row->AugmentDefinition);
		}
	}
}

UGRAugmentDefinition* UGRAugmentSubsystem::GetAugment(FName AugmentID) const
{
	if (auto* Found = AugmentMap.Find(AugmentID))
	{
		return *Found;
	}
	else
	{
		return nullptr;
	}
}

TArray<UGRAugmentDefinition*> UGRAugmentSubsystem::GetRandomAugments(FName CharacterType, int32 Count, AGRPlayerState* PS) const
{
	TArray<UGRAugmentDefinition*> FilteredAugments;

	for (const auto& Pair : AugmentMap)
	{
		if (!Pair.Value)
		{
			continue;
		}

		if (Pair.Value->AugmentCharacterType != CharacterType)
		{
			continue;
		}

		if (PS && PS->GetAugmentLevel(Pair.Value->AugmentID) >= 3)
		{
			continue;
		}

		FilteredAugments.Add(Pair.Value);
	}

	TArray<UGRAugmentDefinition*> ResultAugments;

	while (ResultAugments.Num() < Count && FilteredAugments.Num() > 0)
	{
		int32 Index = FMath::RandRange(0, FilteredAugments.Num() - 1);
		UGRAugmentDefinition* ChosenAugment = FilteredAugments[Index];

		ResultAugments.Add(ChosenAugment);
		FilteredAugments.RemoveAt(Index);
	}

	return ResultAugments;
}

TArray<TObjectPtr<UGRAugmentDefinition>> UGRAugmentSubsystem::GetAugmentValues(FName CharacterType) const
{
	TArray<TObjectPtr<UGRAugmentDefinition>> FilteredAugments;

	for (const auto& Pair : AugmentMap)
	{

		if (!Pair.Value)
		{
			continue;
		}
		
		const UGRAugmentDefinition* AugmentDef = Pair.Value.Get();


		if (AugmentDef->AugmentCharacterType != CharacterType)
		{
			continue;
		}

		FilteredAugments.Add(Pair.Value);
	}

	return FilteredAugments;
}
