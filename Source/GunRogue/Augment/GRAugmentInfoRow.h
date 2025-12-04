// GRAugmentInfoRow.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GRAugmentInfoRow.generated.h"

class UGRAugmentDefinition;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FGRAugmentInfoRow : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AugmentID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UGRAugmentDefinition> AugmentDefinition;
};
