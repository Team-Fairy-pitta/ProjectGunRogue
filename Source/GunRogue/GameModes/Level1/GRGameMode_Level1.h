#pragma once

#include "GameModes/GRGameMode.h"
#include "GameModes/Level1/GRLevel1Data.h"
#include "GRGameMode_Level1.generated.h"


UCLASS()
class GUNROGUE_API AGRGameMode_Level1 : public AGRGameMode
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	FGRLevel1Data Level1Data;
};
