// GRPerkSaveGame

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MetaProgression/GRPerkStructs.h"
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
	TArray<FPerkEntry> PerkInfoRows;

	UPROPERTY()
	int32 MetaGoods = 0;
};
