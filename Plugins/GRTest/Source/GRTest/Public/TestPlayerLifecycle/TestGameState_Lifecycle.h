// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TestGameState_Lifecycle.generated.h"

class ATestCharacter_Lifecycle;

UCLASS()
class GRTEST_API ATestGameState_Lifecycle : public AGameState
{
	GENERATED_BODY()
	
public:

	void UpdateAlivePlayerList();

	const TArray<ATestCharacter_Lifecycle*>& GetAlivePlayers() const
	{
		return AlivePlayers;
	}

private:
	UPROPERTY()
	TArray<ATestCharacter_Lifecycle*> AlivePlayers;
};
