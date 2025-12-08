#pragma once

#include "GameModes/GRGameMode.h"
#include "GameModes/Level1/GRLevel1Data.h"
#include "GRGameMode_Level1.generated.h"


UCLASS()
class GUNROGUE_API AGRGameMode_Level1 : public AGRGameMode
{
	GENERATED_BODY()
	
	friend struct FGRLevel1Data;

public:
	virtual void BeginPlay() override;

	FGRLevel1Node* GetLevel1Node(int32 Index);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue")
	TArray<TSoftObjectPtr<UWorld>> RandomLevelPool;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue")
	TSoftObjectPtr<UWorld> BossLevel;

	UPROPERTY()
	FGRLevel1Data Level1Data;
};
