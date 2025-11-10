#pragma once

#include "GameFramework/GameState.h"
#include "GRGameState.generated.h"

UCLASS()
class GUNROGUE_API AGRGameState : public AGameState
{
	GENERATED_BODY()

public:

	FString GetNextLevelName();

	FName GetPreviousLevelName();

protected:
	int32 Level = 0;
	
};
