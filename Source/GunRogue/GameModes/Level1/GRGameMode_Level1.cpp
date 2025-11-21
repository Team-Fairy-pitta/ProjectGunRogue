#include "GameModes/Level1/GRGameMode_Level1.h"

void AGRGameMode_Level1::BeginPlay()
{
	Super::BeginPlay();

	Level1Data.Init();
}
