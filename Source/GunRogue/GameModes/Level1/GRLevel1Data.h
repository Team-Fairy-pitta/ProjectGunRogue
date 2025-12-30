#pragma once

#include "GRLevel1Data.generated.h"

class AGRGameMode_Level1;

UENUM()
enum class ENodeStatus : uint8
{
	NONE,
	CURRENT,
	NEXT,
	CLEARD
};

UENUM()
enum class ENodeType : uint8
{
	NONE,
	BASE,
	NORMAL,
	HARD,
	BOSS
};


USTRUCT()
struct GUNROGUE_API FGRLevel1Node
{
	GENERATED_BODY()

public:
	FGRLevel1Node();
	void CopyNodeInformation(const FGRLevel1Node& Other);

	UPROPERTY()
	int32 NextLeftIndex;

	UPROPERTY()
	int32 NextRightIndex;

	UPROPERTY()
	ENodeStatus NodeStatus;

	UPROPERTY()
	ENodeType NodeType;

	UPROPERTY()
	TSoftObjectPtr<UWorld> LevelToLoad;
};

USTRUCT()
struct GUNROGUE_API FGRLevel1Data
{
	GENERATED_BODY()

public:
	static int32 RowCount;
	static int32 ColCount;

	FGRLevel1Data();

	void InitAtServer(AGRGameMode_Level1* GRGameMode);
	void InitAtClient();
	bool IsValidData() const { return bIsValid == 1; }

	FGRLevel1Node* GetNode(int32 Index);
	void SetNode(int32 Index, FGRLevel1Node& Data);
	const TArray<FGRLevel1Node>& GetNodes() const { return Nodes; }

	void PrintDebugLog();

private:
	UPROPERTY()
	int8 bIsValid;

	UPROPERTY()
	int32 TotalRoomCount;

	UPROPERTY()
	TArray<FGRLevel1Node> Nodes;

	void MakeAndConnectEmptyRooms();
	void SetupFirstRoom();
	void SetupEachRoomRandomly(AGRGameMode_Level1* GRGameMode);
	void SetupLastRoom(AGRGameMode_Level1* GRGameMode);
};
