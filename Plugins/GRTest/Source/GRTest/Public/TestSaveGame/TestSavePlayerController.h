// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TestSavePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GRTEST_API ATestSavePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// 생성자 또는 BeginPlay에서 입력 바인딩을 설정합니다.
	virtual void SetupInputComponent() override;

protected:
	// 플레이어의 입력(예: F5 키)에 바인딩될 세이브 요청 함수
	UFUNCTION(BlueprintCallable)
	void RequestLocalSave();
	
	// 플레이어의 입력(예: F9 키)에 바인딩될 로드 요청 함수
	UFUNCTION(BlueprintCallable)
	void RequestLocalLoad();

	UFUNCTION(BlueprintImplementableEvent, Category="SaveGame")
	void OnGameDataLoaded(int32 Level, const FString& PlayerName, float HP);
    
private:
	// **임시로 사용할 플레이어 고유 ID 생성 함수 (실제로는 로그인 시스템에서 받아와야 함)**
	FString GetUniquePlayerId() const;
};
