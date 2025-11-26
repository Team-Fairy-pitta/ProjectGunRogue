#include "GameModes/Level1/GRLevel1Data.h"
#include "GameModes/Level1/GRGameMode_Level1.h"

FGRLevel1Node::FGRLevel1Node()
{
	NextLeftIndex = -1;
	NextRightIndex = -1;
	LevelToLoad = nullptr;
	NodeStatus = ENodeStatus::NONE;
}

void FGRLevel1Node::CopyNodeInformation(const FGRLevel1Node& Other)
{
	NextLeftIndex = Other.NextLeftIndex;
	NextRightIndex = Other.NextRightIndex;
	LevelToLoad = Other.LevelToLoad;
}

FGRLevel1Node* FGRLevel1Data::GetNode(int32 Index)
{
	if (Nodes.IsValidIndex(Index))
	{
		return &(Nodes[Index]);
	}
	else
	{
		return nullptr;
	}
}

int32 FGRLevel1Data::RowCount = 4;
int32 FGRLevel1Data::ColCount = 4;

FGRLevel1Data::FGRLevel1Data()
{
	bIsValid = 0;
	TotalRoomCount = RowCount * ColCount;
}

void FGRLevel1Data::InitAtClient()
{
	MakeAndConnectEmptyRooms();
	bIsValid = 1;
}

void FGRLevel1Data::SetNode(int32 Index, FGRLevel1Node& Data)
{
	if (Nodes.IsValidIndex(Index))
	{
		Nodes[Index].CopyNodeInformation(Data);
	}
}

void FGRLevel1Data::InitAtServer(AGRGameMode_Level1* GRGameMode)
{
	if (!IsValid(GRGameMode))
	{
		UE_LOG(LogTemp, Fatal, TEXT("GRGameMode is INVALID"));
		return;
	}

	MakeAndConnectEmptyRooms();
	SetupEachRoomRandomly(GRGameMode);
	bIsValid = 1;
}

void FGRLevel1Data::PrintDebugLog()
{
	if (IsValidData())
	{
		for (int Index = 0; Index < Nodes.Num(); ++Index)
		{
			const FGRLevel1Node& Node = Nodes[Index];
			UE_LOG(LogTemp, Display, TEXT("%d -> [%d, %d]"), Index, Node.NextLeftIndex, Node.NextRightIndex);
			if (Node.LevelToLoad)
			{
				UE_LOG(LogTemp, Display, TEXT("%d Level is %s"), Index, *Node.LevelToLoad->GetName());
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FGRLevel1Data is INVALID"));
	}
}

void FGRLevel1Data::MakeAndConnectEmptyRooms()
{
	// 방을 만들고, 논리적으로 연결한다.
	// 아래와 같은 구조가 되도록...
	//                  -> [3]
	//           -> [2]        -> [7]
	//     -> [1]       -> [6]        -> [B]
	// [0]        -> [5]       -> [A]        -> [F]
	//     -> [4]       -> [9]        -> [E]
	//           -> [8]        -> [D]
	//                  -> [C]
	Nodes.Reserve(TotalRoomCount);
	for (int Loop = 0; Loop < TotalRoomCount; ++Loop)
	{
		Nodes.AddDefaulted();
	}

	for (int Index = 0; Index < TotalRoomCount; ++Index)
	{
		if (Index % ColCount < ColCount - 1)
		{
			Nodes[Index].NextLeftIndex = Index + 1;
		}
		if (Index < (ColCount * (RowCount - 1)))
		{
			Nodes[Index].NextRightIndex = Index + ColCount;
		}
	}
}

void FGRLevel1Data::SetupEachRoomRandomly(AGRGameMode_Level1* GRGameMode)
{
	// 랜덤으로 각 방을 설정한다.
	// 단, 시작맵과 보스 맵은 랜덤으로 지정하지 않는다.
	// 또한, 연결된 방과 같은 맵은 사용하지 않도록 한다.

	if (!IsValid(GRGameMode))
	{
		UE_LOG(LogTemp, Fatal, TEXT("GRGameMode is INVALID"));
		return;
	}

	if (GRGameMode->RandomLevelPool.Num() < 2)
	{
		UE_LOG(LogTemp, Fatal, TEXT("REQUIREMENT: RandomLevelPool.Num() >= 2"));
		return;
	}

	for (int Index = TotalRoomCount - 2; Index > 0; --Index)
	{
		while (true)
		{
			FGRLevel1Node& Node = Nodes[Index];

			int32 RandomMax = GRGameMode->RandomLevelPool.Num();
			int32 Random = FMath::RandRange(0, RandomMax - 1);
			TSoftObjectPtr<UWorld> RandomLevel = GRGameMode->RandomLevelPool[Random];

			if (RandomLevel.IsNull())
			{
				UE_LOG(LogTemp, Fatal, TEXT("GRGameMode->RandomLevelPool[Random] is NULL"));
				return;
			}

			if (Node.NextLeftIndex != -1)
			{
				if (Nodes[Node.NextLeftIndex].LevelToLoad == RandomLevel)
				{
					continue;
				}
			}
			if (Node.NextRightIndex != -1)
			{
				if (Nodes[Node.NextRightIndex].LevelToLoad == RandomLevel)
				{
					continue;
				}
			}

			Node.LevelToLoad = RandomLevel;
			break;
		}
	}
}

