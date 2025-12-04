#pragma once

#include "GameFramework/CheatManager.h"
#include "GRBattleCheatManager.generated.h"

UCLASS()
class GUNROGUE_API UGRBattleCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
public:
	
	// Level에서만 사용 가능. 다음에 로드할 맵을 강제로 바꾼다.
	// 모든 맵이 하나의 맵으로 변경됩니다.
	// 리슨서버의 호스트에서만 사용 가능합니다.
	// 맵 이동 콘솔을 사용하기 전에 사용하세요.
	UFUNCTION(Exec)
	void SetLevel1NextRoomIndex(int32 InIndex);
};
