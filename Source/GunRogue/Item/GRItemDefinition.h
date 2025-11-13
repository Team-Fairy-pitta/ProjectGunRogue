#pragma once

#include "Engine/DataAsset.h"
#include "AbilitySystem/GRAbilitySet.h"
#include "GRItemDefinition.generated.h"

UCLASS()
class GUNROGUE_API UGRItemDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UGRAbilitySet> AbilitySet;
};
