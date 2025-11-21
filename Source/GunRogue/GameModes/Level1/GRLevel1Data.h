#pragma once

#include "GRLevel1Data.generated.h"

USTRUCT(BlueprintType)
struct GUNROGUE_API FGRLevel1Data
{
	GENERATED_BODY()

public:
	void Init();
	bool IsValid() const { return bIsValid; }


	void PrintDebugLog();

private:
	bool bIsValid = false;
};
