#pragma once

#include "AdvancedFriendsGameInstance.h"
#include "GRGameInstance.generated.h"

UCLASS()
class GUNROGUE_API UGRGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	void CreateSession();
};
