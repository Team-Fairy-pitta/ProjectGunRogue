// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TestSaveGame.h"
#include "TestSaveGameInstance.generated.h"


/**
 * 
 */
UCLASS()
class GRTEST_API UTestSaveGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// GameInstance 초기화 시 로드 시도
	virtual void Init() override;

	// 현재 메모리에 로드된 세이브 데이터 (UMySaveGame 타입)
	UPROPERTY()
	UTestSaveGame* CurrentGameData;

	// --- 로컬 플레이어 ID 및 슬롯 관리 ---
private:
	// 모든 플레이어의 기본 세이브 슬롯 이름
	FString GetBaseSaveSlotName() const { return TEXT("MySaveSlot"); }

	// 항상 0을 사용 (멀티플레이어 로컬 세이브에서 UserIndex는 보통 0)
	int32 GetUserIndex() const { return 0; }
    
	// 로컬 플레이어의 고유 ID (예: 로그인 ID 또는 로컬 컨트롤러 ID)
	FString LocalPlayerUniqueId;

public:
	// 로컬 플레이어의 ID를 설정 (가장 중요)
	void SetLocalPlayerUniqueId(const FString& NewId);
    
	// 플레이어 ID를 기반으로 고유한 슬롯 이름을 생성
	FString GetPlayerSpecificSaveSlotName() const;

	// --- 핵심 세이브/로드 로직 ---
public:
	// 세이브 데이터가 없을 경우 기본값으로 CurrentGameData 초기화
	void InitializeDefaultSaveData();

	// 게임 데이터 저장 (클라이언트에서 호출)
	bool SaveGameData(int32 NewLevel, const FString& NewName, float NewHP);

	// 게임 데이터 로드 (클라이언트에서 호출)
	bool LoadGameData();

	// 현재 메모리 및 저장된 파일의 디버그 정보 출력
	void ShowDebugData() const;

	// 세이브 파일 삭제 및 메모리 데이터 초기화
	bool ResetSaveData();
};
