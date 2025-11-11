// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TestSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class GRTEST_API UTestSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	// 이 SaveGame 오브젝트에 저장되는 실제 데이터
    
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerData")
	int32 Level;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerData")
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerData")
	float HP;

	UTestSaveGame()
	{
		// 생성자에서 초기 기본값 설정
		Level = 1;
		PlayerName = TEXT("DefaultHero");
		HP = 100.0f;
	}
};
