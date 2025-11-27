#pragma once

#include "Engine/DataTable.h"
#include "Item/GRItemDefinition.h"
#include "GRItemInfoTableRow.generated.h"

USTRUCT(BlueprintType)
struct FGRItemInfoTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UGRItemDefinition> ItemDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemRarity Rarity;
};


