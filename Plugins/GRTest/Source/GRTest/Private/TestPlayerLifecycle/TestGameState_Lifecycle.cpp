#include "TestPlayerLifecycle/TestGameState_Lifecycle.h"

#include "TestPlayerLifecycle/TestCharacter_Lifecycle.h"
#include "TestPlayerLifecycle/TestPlayerController_Lifecycle.h"


void ATestGameState_Lifecycle::UpdateAlivePlayerList()
{
	if (!HasAuthority())
	{
		return;
	}

	AlivePlayers.Empty();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATestPlayerController_Lifecycle* TestPC = Cast<ATestPlayerController_Lifecycle>(*It);
		if (TestPC)
		{
			ATestCharacter_Lifecycle* TestCharacter = Cast<ATestCharacter_Lifecycle>(TestPC->GetPawn());
			if (TestCharacter)
			{
				AlivePlayers.Add(TestCharacter);
			}
		}
	}
}
