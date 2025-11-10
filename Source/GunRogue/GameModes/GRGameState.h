#pragma once

#include "GameFramework/GameState.h"
#include "GRGameState.generated.h"

UCLASS()
class GUNROGUE_API AGRGameState : public AGameState
{
	GENERATED_BODY()

public:
	FORCEINLINE int32 GetCurrentLevel() const { return Level; }

	void SetCurrentLevel(int32 NewLevel);

protected:
	int32 Level = 0;
	
};
