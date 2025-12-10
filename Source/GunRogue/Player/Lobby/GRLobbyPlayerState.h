#pragma once

#include "Player/GRPlayerState.h"
#include "GRLobbyPlayerState.generated.h"

UCLASS()
class GUNROGUE_API AGRLobbyPlayerState : public AGRPlayerState
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	int32 MetaGoods;

	virtual void InitPerkFromSave() override;
	void SavePerkToSave();

	int32 GetPerkLevel(FName PerkID) const;
	int32 GetMetaGoods() const { return MetaGoods; }

	void SetPerkLevel(FName PerkID, int32 Level);
	void SetMetaGoods(int32 Amount);

	bool TryUpgradePerk(FName PerkID);
};
