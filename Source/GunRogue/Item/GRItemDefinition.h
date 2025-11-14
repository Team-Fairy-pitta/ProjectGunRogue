#pragma once

#include "Engine/DataAsset.h"
#include "AbilitySystem/GRAbilitySet.h"
#include "GRItemDefinition.generated.h"

class UTexture;
class UStaticMesh;

UCLASS()
class GUNROGUE_API UGRItemDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UGRAbilitySet> AbilitySet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture> ItemIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> ItemMesh;
};
