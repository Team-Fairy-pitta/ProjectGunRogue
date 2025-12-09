#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"
#include "UI/Augment/GRAugmentHUDWidget.h"

void AGRBattlePlayerController::RequestApplyAllPerks()
{
	AGRPlayerState* PS = GetPlayerState<AGRPlayerState>();
	if (!PS)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("RequestApplyAllPerks() Called"));

	PS->ServerRPC_ApplyAllPerksToASC();
}

void AGRBattlePlayerController::InitPerks()
{
	AGRPlayerState* GRPlayerState = GetPlayerState<AGRPlayerState>();
	if (!IsValid(GRPlayerState))
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::InitPerks);
		return;
	}
	
	if (GRPlayerState->ArePerksLoaded())
	{
		RequestApplyAllPerks();
	}
	else
	{
		GRPlayerState->OnPerksLoaded.AddDynamic(this, &ThisClass::RequestApplyAllPerks);	
	}
}
