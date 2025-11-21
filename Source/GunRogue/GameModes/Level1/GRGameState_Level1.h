#pragma once

#include "GameModes/GRGameState.h"
#include "GameModes/Level1/GRLevel1Data.h"
#include "GRGameState_Level1.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnReceiveNextRoomInformation)

UCLASS()
class GUNROGUE_API AGRGameState_Level1 : public AGRGameState
{
	GENERATED_BODY()
	
	friend struct FGRLevel1Data;

public:
	AGRGameState_Level1();

	UFUNCTION(Server, Reliable)
	void ServerRPC_RequestNextRoomInformation(int32 CurrentIndex);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_ReceiveNextRoomInformation(int32 Index, FGRLevel1Node LeftRoomInfo, FGRLevel1Node RightRoomInfo);

	UFUNCTION(BlueprintCallable)
	void RequestNextRoomInformation();

	FOnReceiveNextRoomInformation OnReceiveNextRoomInformation;

protected:
	UPROPERTY()
	FGRLevel1Data Level1ClientData; /* GameMode의 그것과는 다르게, 일부 정보만 가지고 있음 */

	int32 CurrentLevel1NodeIndex;
};
