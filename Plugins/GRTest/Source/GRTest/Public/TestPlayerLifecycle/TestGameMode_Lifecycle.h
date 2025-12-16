#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TestGameMode_Lifecycle.generated.h"

/**
 * 
 */
UCLASS()
class GRTEST_API ATestGameMode_Lifecycle : public AGameMode
{
	GENERATED_BODY()
	

public:
	ATestGameMode_Lifecycle();

	void RespawnPlayer(APlayerController* Controller);
};
