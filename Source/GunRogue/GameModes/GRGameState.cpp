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
	int32 PreviousLevel = Level - 3; // 전전 맵을 언로드함 (의도한 사항)
	UE_LOG(LogTemp, Warning, TEXT("Levelinstnacename : %d"),PreviousLevel);
	return *FString::Printf(TEXT("LevelInst_%d"), PreviousLevel);
}
