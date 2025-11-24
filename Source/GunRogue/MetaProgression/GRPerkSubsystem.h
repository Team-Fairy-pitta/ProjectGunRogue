// GRPerkSubsystem.h

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GRPerkSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRPerkSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	void LoadPerks();
	void SavePerks();

	// 멀티플레이어 로컬 세이브에서 UserIndex는 보통 0 
	int32 GetUserIndex() const { return 0; }
	
	FString GetLocalPlayerUniqueId() const;
	
	FString GetPlayerSpecificSaveSlotName(const FString& PlayerId) const;

private:
	FString LocalPlayerUniqueId;

public:
	int32 GetPerkLevel(FName PerkID) const;
	void SetPerkLevel(FName PerkID, int32 Level);

	float GetPerkBonus(FName PerkID, const UDataTable* PerkTable) const;

	int32 GetMetaGoods() const { return MetaGoods;}
	void SetMetaGoods(int32 Amount);
	void AddMetaGoods(int32 Amount);
	
	bool TryUpgradePerk(FName PerkID, const UDataTable* PerkTable);

private:
	UPROPERTY()
	TMap<FName, int32> PerkInfoRows;

	int32 MetaGoods;
};
