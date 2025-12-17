#include "GameModes/Level1/GRGameState_Level1.h"
#include "GameModes/Level1/GRGameMode_Level1.h"
#include "GameModes/Level1/GRLevel1Data.h"
#include "Net/UnrealNetwork.h"

AGRGameState_Level1::AGRGameState_Level1()
{
	CurrentLevel1NodeIndex = 0; // 항상 0번째 방에서 시작
}

void AGRGameState_Level1::BeginPlay()
{
	Super::BeginPlay();

	Level1ClientData.InitAtClient();

	if (HasAuthority())
	{
		RequestNextRoomInformation();
	}
}

void AGRGameState_Level1::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentLevel1NodeIndex);
	DOREPLIFETIME(ThisClass, Level1ClientData);
}

void AGRGameState_Level1::SetCurrentRoomIndex(int32 InIndex)
{
	FGRLevel1Node* CurrentRoom = Level1ClientData.GetNode(CurrentLevel1NodeIndex);
	Level1ClientData.GetNode(CurrentLevel1NodeIndex)->NodeStatus = ENodeStatus::CLEARD;

	int32 LeftRoomIndex = CurrentRoom->NextLeftIndex;
	int32 RightRoomIndex = CurrentRoom->NextRightIndex;
	if (LeftRoomIndex != -1)
	{
		Level1ClientData.GetNode(LeftRoomIndex)->NodeStatus = ENodeStatus::NONE;
	}
	if (RightRoomIndex != -1)
	{
		Level1ClientData.GetNode(RightRoomIndex)->NodeStatus = ENodeStatus::NONE;
	}
	
	CurrentLevel1NodeIndex = InIndex;
}

void AGRGameState_Level1::RequestNextRoomInformation()
{
	if (!HasAuthority())
	{
		return;
	}

	if (!GetWorld())
	{
		return;
	}

	AGRGameMode_Level1* GRGameMode = GetWorld()->GetAuthGameMode<AGRGameMode_Level1>();
	if (!IsValid(GRGameMode))
	{
		UE_LOG(LogTemp, Fatal, TEXT("GRGameMode is INVALID"));
		return;
	}

	FGRLevel1Node* CurrentRoom = GRGameMode->GetLevel1Node(CurrentLevel1NodeIndex);
	if (!CurrentRoom)
	{
		UE_LOG(LogTemp, Fatal, TEXT("CurrentRoom is INVALID"));
		return;
	}

	FGRLevel1Node* LeftRoom = nullptr;
	FGRLevel1Node* RightRoom = nullptr;
	int32 LeftRoomIndex = CurrentRoom->NextLeftIndex;
	int32 RightRoomIndex = CurrentRoom->NextRightIndex;
	if (LeftRoomIndex != -1)
	{
		LeftRoom = GRGameMode->GetLevel1Node(LeftRoomIndex);
		Level1ClientData.SetNode(LeftRoomIndex, *LeftRoom);
		Level1ClientData.GetNode(LeftRoomIndex)->NodeStatus = ENodeStatus::NEXT;
	}
	if (RightRoomIndex != -1)
	{
		RightRoom = GRGameMode->GetLevel1Node(RightRoomIndex);
		Level1ClientData.SetNode(RightRoomIndex, *RightRoom);
		Level1ClientData.GetNode(RightRoomIndex)->NodeStatus = ENodeStatus::NEXT;
	}
	Level1ClientData.GetNode(CurrentLevel1NodeIndex)->NodeStatus = ENodeStatus::CURRENT;
}

FGRLevel1Node* AGRGameState_Level1::GetCurrentNodeInfo()
{
	return Level1ClientData.GetNode(CurrentLevel1NodeIndex);
}
