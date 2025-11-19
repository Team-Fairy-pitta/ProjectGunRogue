#include "GameModes/GRGameState.h"

FString AGRGameState::GetNextLevelName()
{
	int32 CurrentLevel = Level;
	Level++;
	UE_LOG(LogTemp, Warning, TEXT("Level : %d"),CurrentLevel);
	return FString::Printf(TEXT("LevelInst_%d"), CurrentLevel);;
}

FName AGRGameState::GetPreviousLevelName()
{
	int32 PreviousLevel = Level - 2;
	UE_LOG(LogTemp, Warning, TEXT("Levelinstnacename : %d"),PreviousLevel);
	return *FString::Printf(TEXT("LevelInst_%d"), PreviousLevel);
}
