#include "Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"

#pragma region Goods
void AGRPlayerState::AddMetaGoods(int32 Amount)
{
	if (!HasAuthority())
	{
		return;
	}

	CurrentMetaGoods += Amount;
	OnRep_CurrentMetaGoods();
}

void AGRPlayerState::AddGold(int32 Amount)
{
	if (!HasAuthority())
	{
		return;
	}

	Gold += Amount;
	OnRep_Gold();
}

void AGRPlayerState::OnRep_CurrentMetaGoods()
{
	UpdateMetaGoodsUI();
	SavePerkToSave();
}

void AGRPlayerState::OnRep_Gold()
{
	UpdateGoldUI();
}

void AGRPlayerState::UpdateMetaGoodsUI()
{
	AGRBattlePlayerController* BattlePlayerController = GetOwner<AGRBattlePlayerController>();
	if (!IsValid(BattlePlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("GetOwner() is NOT AGRBattlePlayerController"));
		return;
	}

	BattlePlayerController->SyncMetaGoodsUI();
}

void AGRPlayerState::UpdateGoldUI()
{
	AGRBattlePlayerController* BattlePlayerController = GetOwner<AGRBattlePlayerController>();
	if (!IsValid(BattlePlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("GetOwner() is NOT AGRBattlePlayerController"));
		return;
	}

	BattlePlayerController->SyncGoldUI();
}
#pragma endregion

