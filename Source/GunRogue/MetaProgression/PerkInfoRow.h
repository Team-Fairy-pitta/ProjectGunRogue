// PerkInfoRow.h

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "PerkInfoRow.generated.h"

class UTexture2D;
/**
 * 
 */
UENUM(BlueprintType)
enum class EPerkModifierOpType : uint8
{
	Add,
	Multiply,
	InverseMultiply,
	Divide
};


USTRUCT(BlueprintType)
struct FPerkInfoRow : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText PerkName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PerkID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText PerkDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> PerkIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxLevel = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ValuePerLevel = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CostPerLevel = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PerkCategory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag PerkTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPerkModifierOpType PerkModifierOp;
};
