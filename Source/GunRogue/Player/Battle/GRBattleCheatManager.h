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

	// Level에서만 사용 가능. 다음에 로드할 맵을 강제로 바꾼다.
	// 모든 맵이 보스맵으로 바뀐다.
	UFUNCTION(Exec)
	void SetLevel1NextRoomBoss();

	// 특정 Index의 Player의 이름을 알아낸다.
	UFUNCTION(Exec)
	void GetPlayerName(int32 InIndex);

	// 특정 Index의 Player의 체력을 0으로 만든다.
	UFUNCTION(Exec)
	void KillPlayer(int32 InIndex);

	// 특정 Index의 Playey를 리스폰한다.
	UFUNCTION(Exec)
	void RespawnPlayer(int32 InIndex);

	// 죽어있는 모든 Playey를 리스폰한다.
	UFUNCTION(Exec)
	void RespawnAllDeadPlayer();

	// 모든 AI를 처치한다.
	UFUNCTION(Exec)
	void KillAllAI();

	// 메시지 브로드캐스트 테스트
	UFUNCTION(Exec)
	void BroadcastMessage(FString Message,float ShowMessageTime);
};
