#pragma once

#include "GameFramework/CheatManager.h"
#include "GRStartCheatManager.generated.h"

UCLASS()
class GUNROGUE_API UGRStartCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec)
	void ConnectToServer(const FString& IPAddress);

	UFUNCTION(Exec)
	void ConnectToServerWithPort(const FString& IPAddress, int32 Port);
};
