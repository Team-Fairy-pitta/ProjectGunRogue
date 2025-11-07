#include "TestCheatManager/TestPlayerController_Cheat.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TestCheatManager/TestCheatManager.h"

ATestPlayerController_Cheat::ATestPlayerController_Cheat()
{
	CheatClass = UTestCheatManager::StaticClass();
}
