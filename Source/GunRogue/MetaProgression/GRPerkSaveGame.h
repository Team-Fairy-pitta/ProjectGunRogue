// GRPerkSaveGame

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GRPerkSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRPerkSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<FName, int32> PerkInfoRows;

	UPROPERTY()
	int32 MetaGoods = 0;
};
