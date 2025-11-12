#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TestEnemySpawnGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GRTEST_API ATestEnemySpawnGameMode : public AGameMode
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void Spawn();

};
