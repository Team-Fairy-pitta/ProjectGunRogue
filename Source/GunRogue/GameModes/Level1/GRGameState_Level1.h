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
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void SetCurrentRoomIndex(int32 InIndex);
	void RequestNextRoomInformation();

	const FGRLevel1Data& GetLevel1ClientData() const { return Level1ClientData; }
	FGRLevel1Node* GetCurrentNodeInfo();

protected:
	UPROPERTY(Replicated)
	int32 CurrentLevel1NodeIndex;

	UPROPERTY(Replicated)
	FGRLevel1Data Level1ClientData; /* GameMode의 그것과는 다르게, 일부 정보만 가지고 있음 */
};
