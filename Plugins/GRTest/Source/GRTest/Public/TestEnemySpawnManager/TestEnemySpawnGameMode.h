// Fill out your copyright notice in the Description page of Project Settings.

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
};
