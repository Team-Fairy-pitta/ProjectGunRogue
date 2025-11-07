#pragma once

#include "Engine/DataAsset.h"
#include "TestItemDataAsset_Cheat.generated.h"

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSubclassOf<AActor> ItemClass;
};


UCLASS()
class GRTEST_API UTestItemDataAsset_Cheat : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TArray<FItemData> Items;
	
};
