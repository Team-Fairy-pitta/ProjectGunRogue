// GRPerkSubsystem.h

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GRPerkSubsystem.generated.h"

struct FPerkEntry;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRPerkSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	bool LoadPerks(const FString& PlayerID, TArray<FPerkEntry>& InPerkInfoRows, int32& InMetaGoods);
	void SavePerks(const FString& PlayerID, const TArray<FPerkEntry>& InPerkInfoRows, int32& InMetaGoods);

	// 멀티플레이어 로컬 세이브에서 UserIndex는 보통 0 
	int32 GetUserIndex() const { return 0; }
	
	FString GetPlayerSpecificSaveSlotName(const FString& PlayerId) const;

	UDataTable* GetPerkTable() const { return PerkTable; }

	UPROPERTY()
	FSoftObjectPath PerkDataTablePath;

	UPROPERTY()
	UDataTable* PerkTable;
};
