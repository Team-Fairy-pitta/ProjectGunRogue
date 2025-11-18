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

	int32 GetUserIndex() const { return 0; }

	FString GetBaseSaveSlotName() const { return TEXT("SlotName"); }
	FString GetPlayerSpecificSaveSlotName() const;

	void SetLocalPlayerUniqueId(const FString& NewId);
	
private:
	FString LocalPlayerUniqueId;

public:
	int32 GetPerkLevel(FName PerkName) const;
	void SetPerkLevel(FName PerkName, int32 Level);

	float GetPerkBonus(FName PerkName, const UDataTable* PerkTable) const;

	int32 GetMetaGoods() const { return MetaGoods;}
	void AddMetaGoods(int32 Amount);

	bool TryUpgradePerk(FName PerkName, const UDataTable* PerkTable);

private:
	UPROPERTY()
	TMap<FName, int32> PerkLevels;

	int32 MetaGoods;
};
