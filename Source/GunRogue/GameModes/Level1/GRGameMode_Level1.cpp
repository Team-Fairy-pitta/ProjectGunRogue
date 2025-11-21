#include "GameModes/Level1/GRGameMode_Level1.h"

void AGRGameMode_Level1::BeginPlay()
{
	Super::BeginPlay();

	Level1Data.Init(this);

#if WITH_EDITOR
	Level1Data.PrintDebugLog();
#endif
}
