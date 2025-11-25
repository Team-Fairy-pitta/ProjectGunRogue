#pragma once

#include "GRLevel1Data.generated.h"

class AGRGameMode_Level1;

enum class ENodeStatus : uint8
{
	NONE,
	CURRENT,
	NEXT,
	CLEARD
};

USTRUCT()
struct GUNROGUE_API FGRLevel1Node
{
	GENERATED_BODY()

public:
	FGRLevel1Node();
	void CopyNodeInformation(const FGRLevel1Node& Other);

	int32 NextLeftIndex;
	int32 NextRightIndex;

	ENodeStatus NodeStatus;

	UPROPERTY()
	TObjectPtr<UWorld> LevelToLoad;
};

USTRUCT()
struct GUNROGUE_API FGRLevel1Data
{
	GENERATED_BODY()

public:
	FGRLevel1Data();
	void InitAtServer(AGRGameMode_Level1* GRGameMode);
	void InitAtClient();
	bool IsValidData() const { return bIsValid; }
	FGRLevel1Node* GetNode(int32 Index);
	void SetNode(int32 Index, FGRLevel1Node& Data);

	const TArray<FGRLevel1Node>& GetNodes() const { return Nodes; }

	void PrintDebugLog();

private:
	bool bIsValid;
	int32 TotalRoomCount;

	UPROPERTY()
	TArray<FGRLevel1Node> Nodes;

	void MakeAndConnectEmptyRooms(int32 RoomNum);
	void SetupEachRoomRandomly(AGRGameMode_Level1* GRGameMode, int32 RoomNum);
};
