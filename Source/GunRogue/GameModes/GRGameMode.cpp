#include "GameModes/GRGameMode.h"
#include "GameModes/GRGameState.h"
#include "Character/GRCharacter.h"
#include "Player/GRPlayerController.h"
#include "Player/GRPlayerState.h"


AGRGameMode::AGRGameMode()
{
	// 기본 클래스 지정
	// 그대로 사용하면 안되며, 상속받은 Blueprint Class를 사용해야 합니다.
	DefaultPawnClass = AGRCharacter::StaticClass();
	GameStateClass = AGRGameState::StaticClass();
	PlayerControllerClass = AGRPlayerController::StaticClass();
	PlayerStateClass = AGRPlayerState::StaticClass();
}
