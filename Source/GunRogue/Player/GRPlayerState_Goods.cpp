#include "Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"

#pragma region Goods
void AGRPlayerState::AddMetaGoods(int32 Amount)
{
	UE_LOG(LogTemp, Display, TEXT("AddMetaGoods1 : Current Meta Goods %d"), CurrentMetaGoods);
	
	if (HasAuthority())
	{
		CurrentMetaGoods += Amount;
		UE_LOG(LogTemp, Display, TEXT("AddMetaGoods2 : Current Meta Goods %d"), CurrentMetaGoods);
		
		OnRep_CurrentMetaGoods();
	}
	else
	{
		ServerRPC_AddMetaGoods(Amount);
	}
}

void AGRPlayerState::ServerRPC_AddMetaGoods_Implementation(int32 Amount)
{
	CurrentMetaGoods += Amount;
	UE_LOG(LogTemp, Display, TEXT("AddMetaGoods3 : Current Meta Goods %d"), CurrentMetaGoods);
	
	OnRep_CurrentMetaGoods();
}

void AGRPlayerState::AddGold(int32 Amount)
{
	if (HasAuthority())
	{
		Gold += Amount;
		OnRep_Gold();
	}
	else
	{
		ServerRPC_AddGold(Amount);
	}
}

void AGRPlayerState::ServerRPC_AddGold_Implementation(int32 Amount)
{
	Gold += Amount;
	OnRep_Gold();
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

void AGRPlayerState::OnRep_CurrentMetaGoods()
{
	UpdateMetaGoodsUI();
}

void AGRPlayerState::OnRep_Gold()
{
	UpdateGoldUI();
}

#pragma endregion

