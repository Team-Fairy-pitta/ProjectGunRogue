#include "GameModes/GRGameState.h"

FString AGRGameState::GetNextLevelName()
{
	int32 CurrentLevel = Level;
	Level++;
	return FString::Printf(TEXT("LevelInst_%d"), CurrentLevel);;
}

FName AGRGameState::GetPreviousLevelName()
{
	int32 PreviousLevel = Level - 2;
	return *FString::Printf(TEXT("LevelInst_%d"), PreviousLevel);
}
