#include "GameModes/Level1/GRGameMode_Level1.h"

void AGRGameMode_Level1::BeginPlay()
{
	Super::BeginPlay();

	Level1Data.InitAtServer(this);

#if WITH_EDITOR
	Level1Data.PrintDebugLog();
#endif
}

FGRLevel1Node* AGRGameMode_Level1::GetLevel1Node(int32 Index)
{
	if (Level1Data.IsValidData())
	{
		return Level1Data.GetNode(Index);
	}
	else
	{
		return nullptr;
	}
}
