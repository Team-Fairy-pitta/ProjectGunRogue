#pragma once

#include "GameFramework/CheatManager.h"
#include "GRLobbyCheatManager.generated.h"

UCLASS()
class GUNROGUE_API UGRLobbyCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	// 성장 재화의 양을 강제로 지정한다.
	UFUNCTION(Exec)
	void SetMetaGoods(int32 InValue);
};
