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

	APawn* OldPawn = Controller->GetPawn();
	if (OldPawn)
	{
		OldPawn->Destroy();
	}

	RestartPlayer(Controller);

	if (Controller->GetPawn())
	{
		Controller->SetViewTarget(Controller->GetPawn());
	}
}
