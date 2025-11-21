#pragma once

#include "GRLevel1Data.generated.h"

class AGRGameMode_Level1;

USTRUCT()
struct GUNROGUE_API FGRLevel1Node
{
	GENERATED_BODY()

public:
	FGRLevel1Node();

	int32 NextLeftIndex;
	int32 NextRightIndex;

	UPROPERTY()
	TObjectPtr<UWorld> LevelToLoad;
};

USTRUCT()
struct GUNROGUE_API FGRLevel1Data
{
	GENERATED_BODY()

public:
	FGRLevel1Data();
	void Init(AGRGameMode_Level1* GRGameMode);
	bool IsValidData() const { return bIsValid; }

	void PrintDebugLog();

private:
	bool bIsValid;

	UPROPERTY()
	TArray<FGRLevel1Node> Nodes;
};
