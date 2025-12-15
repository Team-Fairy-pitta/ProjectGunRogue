#include "TestPlayerLifecycle/TestGameMode_Lifecycle.h"

ATestGameMode_Lifecycle::ATestGameMode_Lifecycle()
{
}

void ATestGameMode_Lifecycle::RespawnPlayer(APlayerController* Controller)
{
	if (!Controller)
	{
		return;
	}

	RestartPlayer(Controller);

	if (Controller->GetPawn())
	{
		Controller->SetViewTarget(Controller->GetPawn());
	}
}
