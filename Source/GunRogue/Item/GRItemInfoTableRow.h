#pragma once

#include "Engine/DataTable.h"
#include "GRItemInfoTableRow.generated.h"

class UGRItemDefinition;

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	NONE,
	NORMAL UMETA(DisplayName = "Normal Item"),
	RARE UMETA(DisplayName = "Rare Item"),
	EPIC UMETA(DisplayName = "Epic Item")
};

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


