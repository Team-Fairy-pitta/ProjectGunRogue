// Fill out your copyright notice in the Description page of Project Settings.


#include "TestSaveGame/TestSavePlayerController.h"
#include "TestSaveGame/TestSaveGameInstance.h" // UMyGameInstance 포함
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

void ATestSavePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		// --- 핵심 바인딩 코드 ---
        
		// "SaveGame" 이라는 이름의 액션이 '눌렸을 때 (IE_Pressed)', RequestLocalSave 함수 실행
		InputComponent->BindAction("SaveGame", EInputEvent::IE_Pressed, this, &ATestSavePlayerController::RequestLocalSave);
		
		// "LoadGame" 이라는 이름의 액션이 '눌렸을 때 (IE_Pressed)', RequestLocalLoad 함수 실행
		InputComponent->BindAction("LoadGame", EInputEvent::IE_Pressed, this, &ATestSavePlayerController::RequestLocalLoad);
        
		// --- 바인딩 끝 ---
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController: InputComponent is NULL. Cannot bind actions."));
	}
}

FString ATestSavePlayerController::GetUniquePlayerId() const
{
    // WARNING: 이 함수는 실제 온라인 게임에서 사용하기 부적합합니다.
    // 실제로는 Online Subsystem을 통해 로그인된 유저의 고유 ID(FUniqueNetId)를 사용해야 합니다.
    // 여기서는 테스트를 위해 로컬 컨트롤러 ID를 임시 ID로 사용합니다.

	// 1. PlayerState의 GetPlayerId()를 사용하여 세션 내 고유 ID를 얻습니다.
	if (PlayerState)
	{
		// PlayerState의 ID는 서버가 할당하는 고유한 네트워크 ID입니다 (0부터 시작).
		// 이 ID가 PIE 환경에서 ControllerId보다 더 잘 작동할 때가 많습니다.
		return FString::Printf(TEXT("NetID_%d"), PlayerState->GetPlayerId());
	}

	// 2. PlayerState가 없다면 (매우 드문 경우), 로컬 ControllerId를 Fallback으로 사용합니다.
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		return FString::Printf(TEXT("ControllerID_Fallback_%d"), LocalPlayer->GetControllerId());
	}
    
	// 최종 실패
	return TEXT("Unknown_Error_ID");
}


void ATestSavePlayerController::RequestLocalSave()
{
    // **1. 이 코드가 클라이언트에서 실행되고 있는지 확인 (매우 중요)**
    // GetLocalRole()이 ROLE_Authority가 아닌 클라이언트에서만 실행되어야 합니다. 
    // 하지만 PlayerController는 로컬 플레이어에게만 입력 처리를 제공하므로 IsLocalController()를 사용합니다.
    if (!IsLocalController()) 
    {
        // 서버에서 이 함수가 호출되는 것을 방지
        return;
    }
    
    // 2. GameInstance 가져오기
    UTestSaveGameInstance* GameInstance = GetGameInstance<UTestSaveGameInstance>();

    if (GameInstance)
    {
        // 3. 고유 ID 설정
        FString PlayerID = GetUniquePlayerId();
        GameInstance->SetLocalPlayerUniqueId(PlayerID);

        // 4. 저장할 데이터 가져오기 (예시)
        // 실제로는 플레이어 캐릭터나 상태 객체에서 Level, Name, HP를 가져와야 합니다.
        int32 CurrentLevel = 5; 
        FString CurrentName = FString::Printf(TEXT("Player_%s"), *PlayerID);
        float CurrentHP = 85.5f;

        // 5. 세이브 로직 호출
        if (GameInstance->SaveGameData(CurrentLevel, CurrentName, CurrentHP))
        {
            UE_LOG(LogTemp, Warning, TEXT("[CLIENT] Local Save Initiated and Completed Successfully!"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[CLIENT] Local Save Failed!"));
        }
    }
}


void ATestSavePlayerController::RequestLocalLoad()
{
	if (!IsLocalController())
	{
		return;
	}

	UTestSaveGameInstance* GameInstance = GetGameInstance<UTestSaveGameInstance>();

	if (GameInstance)
	{
		// 1. 로드하기 전에 고유 ID 설정 (필수)
		FString PlayerID = GetUniquePlayerId();
		GameInstance->SetLocalPlayerUniqueId(PlayerID);

        
		// 2. 로드 로직 호출
		if (GameInstance->LoadGameData())
		{
			// --- 3. 핵심 수정 부분: 로드된 데이터를 블루프린트로 전달 ---
			if (GameInstance->CurrentGameData)
			{
				// 로드 성공 후, 저장된 데이터를 블루프린트 이벤트로 전달!
				// PlayerController 블루프린트에서 이 이벤트를 받아서 처리합니다.
				OnGameDataLoaded(
					GameInstance->CurrentGameData->Level, 
					GameInstance->CurrentGameData->PlayerName, 
					GameInstance->CurrentGameData->HP
				);
			}
			// -----------------------------------------------------------
            
			// 화면 디버그 메시지로 변경 (이전 답변 참고)
			FString SuccessMessage = FString::Printf(
				TEXT("LOAD SUCCESS! Name: %s"), 
				*GameInstance->CurrentGameData->PlayerName);
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, SuccessMessage);
            
			// UE_LOG(LogTemp, Warning, TEXT("[CLIENT] Local Load Completed. Data applied to CurrentGameData."));
			// GameInstance->ShowDebugData();
		}
		else
		{
			// 화면 디버그 메시지로 변경 (이전 답변 참고)
			FString FailMessage = TEXT("LOAD FAILED or Default Data Initialized.");
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FailMessage);
            
			// UE_LOG(LogTemp, Error, TEXT("[CLIENT] Local Load Failed or Default Data Initialized."));
		}
	}
}
