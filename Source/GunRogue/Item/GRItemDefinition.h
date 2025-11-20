#pragma once

#include "Engine/DataAsset.h"
#include "AbilitySystem/GRAbilitySet.h"
#include "GRItemDefinition.generated.h"

class UTexture2D;
class UStaticMesh;

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	NONE,
	NORMAL UMETA(DisplayName = "Normal Item"),
	RARE UMETA(DisplayName = "Rare Item"),
	EPIC UMETA(DisplayName = "Epic Item")
};


UCLASS()
class GUNROGUE_API UGRItemDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UGRAbilitySet> AbilitySet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> ItemIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ItemDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> ItemMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EItemRarity Rarity;
};
