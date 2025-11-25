#include "GameModes/Level1/GRGameState_Level1.h"
#include "GameModes/Level1/GRGameMode_Level1.h"
#include "GameModes/Level1/GRLevel1Data.h"
#include "Net/UnrealNetwork.h"

AGRGameState_Level1::AGRGameState_Level1()
{
	Level1ClientData.InitAtClient();

	// 항상 0번째 방에서 시작
	CurrentLevel1NodeIndex = 0;
}

void AGRGameState_Level1::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentLevel1NodeIndex);
}

void AGRGameState_Level1::RequestNextRoomInformation()
{
	ServerRPC_RequestNextRoomInformation(CurrentLevel1NodeIndex);
}


void AGRGameState_Level1::ServerRPC_RequestNextRoomInformation_Implementation(int32 CurrentIndex)
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

	FGRLevel1Node* CurrentRoom = GRGameMode->GetLevel1Node(CurrentIndex);
	if (!CurrentRoom)
	{
		UE_LOG(LogTemp, Fatal, TEXT("CurrentRoom is INVALID"));
		return;
	}

	FGRLevel1Node* LeftRoom = nullptr;
	FGRLevel1Node* RightRoom = nullptr;
	if (CurrentRoom->NextLeftIndex != -1)
	{
		LeftRoom = GRGameMode->GetLevel1Node(CurrentRoom->NextLeftIndex);
	}
	if (CurrentRoom->NextRightIndex != -1)
	{
		RightRoom = GRGameMode->GetLevel1Node(CurrentRoom->NextRightIndex);
	}

	MulticastRPC_ReceiveNextRoomInformation(CurrentIndex, *LeftRoom, *RightRoom);
}

void AGRGameState_Level1::MulticastRPC_ReceiveNextRoomInformation_Implementation(int32 Index, FGRLevel1Node LeftRoomInfo, FGRLevel1Node RightRoomInfo)
{
	FGRLevel1Node* CurrentRoom = Level1ClientData.GetNode(Index);
	if (!CurrentRoom)
	{
		return;
	}

	int32 LeftRoomIndex = CurrentRoom->NextLeftIndex;
	int32 RightRoomIndex = CurrentRoom->NextRightIndex;

	if (LeftRoomIndex != -1)
	{
		Level1ClientData.SetNode(LeftRoomIndex, LeftRoomInfo);
	}
	if (RightRoomIndex != -1)
	{
		Level1ClientData.SetNode(RightRoomIndex, RightRoomInfo);
	}

	Level1ClientData.GetNode(Index)->NodeStatus = ENodeStatus::CURRENT;
	Level1ClientData.GetNode(LeftRoomIndex)->NodeStatus = ENodeStatus::NEXT;
	Level1ClientData.GetNode(RightRoomIndex)->NodeStatus = ENodeStatus::NEXT;


#if WITH_EDITOR
	Level1ClientData.PrintDebugLog();
#endif

	if (OnReceiveNextRoomInformation.IsBound())
	{
		OnReceiveNextRoomInformation.Broadcast();
	}
}
