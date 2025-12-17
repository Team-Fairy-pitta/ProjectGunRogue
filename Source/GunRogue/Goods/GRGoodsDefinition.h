// GRGoodsDefinition.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GRGoodsDefinition.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRGoodsDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName GoodsType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> GoodsMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 GoodsAmount = 1;
};
