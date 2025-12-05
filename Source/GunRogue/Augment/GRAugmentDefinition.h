// GRAugmentDefinition.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GRAugmentStructs.h"
#include "GRAugmentDefinition.generated.h"

class UTexture2D;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRAugmentDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AugmentID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText AugmentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText AugmentDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> AugmentIcon;

	//캐릭터 구분용
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AugmentCharacterType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AugmentCategory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText AugmentCategoryName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAugmentValues> AugmentValues;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAugmentAdditionalValue> AugmentAdditionalValues;
};
