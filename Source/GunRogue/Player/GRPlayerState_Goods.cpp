#include "Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"

#pragma region Goods
void AGRPlayerState::AddMetaGoods(int32 Amount)
{
	CurrentMetaGoods += Amount;
}

void AGRPlayerState::AddGold(int32 Amount)
{
	Gold += Amount;
}

void AGRPlayerState::OnRep_CurrentMetaGoods()
{
	AGRBattlePlayerController* BattlePlayerController = GetOwner<AGRBattlePlayerController>();
	if (!IsValid(BattlePlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("GetOwner() is NOT AGRBattlePlayerController"));
		return;
	}

	BattlePlayerController->UpdateUICurrentMetaGoods();
}

void AGRPlayerState::OnRep_Gold()
{
	AGRBattlePlayerController* BattlePlayerController = GetOwner<AGRBattlePlayerController>();
	if (!IsValid(BattlePlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("GetOwner() is NOT AGRBattlePlayerController"));
		return;
	}

	BattlePlayerController->UpdateUIGold();
}

#pragma endregion

