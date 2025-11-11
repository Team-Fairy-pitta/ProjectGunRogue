// Fill out your copyright notice in the Description page of Project Settings.


#include "TestSaveGame/TestSaveGameInstance.h"

#include "Kismet/GameplayStatics.h"

// --- ID 및 슬롯 관리 구현 ---

void UTestSaveGameInstance::SetLocalPlayerUniqueId(const FString& NewId)
{
    LocalPlayerUniqueId = NewId;
    UE_LOG(LogTemp, Log, TEXT("GameInstance: Local Player Unique ID set to: %s"), *LocalPlayerUniqueId);
}

FString UTestSaveGameInstance::GetPlayerSpecificSaveSlotName() const
{
    // 로컬 ID가 없으면 경고 후 기본 슬롯 이름 반환 (치명적인 오류 방지)
    if (LocalPlayerUniqueId.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance: LocalPlayerUniqueId is not set. Using base slot name only."));
        return GetBaseSaveSlotName();
    }
    // "MySaveSlot_UniquePlayerID" 형식으로 고유한 슬롯 이름 생성
    return FString::Printf(TEXT("%s_%s"), *GetBaseSaveSlotName(), *LocalPlayerUniqueId);
}

// --- 핵심 로직 구현 ---

void UTestSaveGameInstance::Init()
{
    Super::Init();
    
    // NOTE: Init()에서는 LocalPlayerUniqueId를 알 수 없으므로,
    // 이 시점의 LoadGameData() 호출은 잠재적인 오류를 발생시킬 수 있습니다.
    // 멀티플레이어 환경에서는 ID 설정 후 (예: 로그인 완료 후) 별도로 LoadGameData()를 호출해야 합니다.
    
    // 싱글플레이어 호환을 위해 Base Slot Name으로 로드를 시도합니다.
    // 멀티플레이어 클라이언트라면 이 로직은 무시됩니다.
    // if (!LoadGameData())
    // {
    //     InitializeDefaultSaveData();
    // }
}

void UTestSaveGameInstance::InitializeDefaultSaveData()
{
    CurrentGameData = Cast<UTestSaveGame>(UGameplayStatics::CreateSaveGameObject(UTestSaveGame::StaticClass()));
    
    if (CurrentGameData)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameInstance: New default save data initialized."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance: Failed to create default UMySaveGame object!"));
    }
}

bool UTestSaveGameInstance::SaveGameData(int32 NewLevel, const FString& NewName, float NewHP)
{
    if (LocalPlayerUniqueId.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("SAVE FAILED: LocalPlayerUniqueId is not set. Cannot save."));
        return false;
    }
    
    if (!CurrentGameData)
    {
        InitializeDefaultSaveData();
        if (!CurrentGameData) return false;
    }

    CurrentGameData->Level = NewLevel;
    CurrentGameData->PlayerName = NewName;
    CurrentGameData->HP = NewHP;

    FString SlotName = GetPlayerSpecificSaveSlotName();

    if (UGameplayStatics::SaveGameToSlot(CurrentGameData, SlotName, GetUserIndex()))
    {
        UE_LOG(LogTemp, Log, TEXT("SAVE SUCCESS: Slot: '%s', Level: %d, Name: %s."), 
            *SlotName, NewLevel, *NewName);
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SAVE FAILED: Could not save game to slot '%s'."), *SlotName);
        return false;
    }
}

bool UTestSaveGameInstance::LoadGameData()
{
    if (LocalPlayerUniqueId.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("LOAD ABORTED: LocalPlayerUniqueId is not set. Cannot load specific slot."));
        // ID가 없으면 로드할 수 없으므로 기본 데이터로 초기화
        InitializeDefaultSaveData(); 
        return false;
    }
    
    FString SlotName = GetPlayerSpecificSaveSlotName();

    if (UGameplayStatics::DoesSaveGameExist(SlotName, GetUserIndex()))
    {
        USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SlotName, GetUserIndex());
        UTestSaveGame* LoadedMySaveGame = Cast<UTestSaveGame>(LoadedGame);

        if (LoadedMySaveGame)
        {
            CurrentGameData = LoadedMySaveGame;
            UE_LOG(LogTemp, Log, TEXT("LOAD SUCCESS: Slot: '%s', Level: %d, Name: %s."), 
                *SlotName, CurrentGameData->Level, *CurrentGameData->PlayerName);
            return true;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("LOAD FAILED: Slot '%s' exists but failed to cast to UMySaveGame."), *SlotName);
            InitializeDefaultSaveData();
            return false;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadGameData: Save file does not exist in slot '%s'. Initializing default."), *SlotName);
        InitializeDefaultSaveData();
        return false;
    }
}

bool UTestSaveGameInstance::ResetSaveData()
{
    if (LocalPlayerUniqueId.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("RESET ABORTED: LocalPlayerUniqueId is not set. Cannot delete specific slot."));
        return false;
    }
    
    FString SlotName = GetPlayerSpecificSaveSlotName();

    if (UGameplayStatics::DoesSaveGameExist(SlotName, GetUserIndex()))
    {
        if (UGameplayStatics::DeleteGameInSlot(SlotName, GetUserIndex()))
        {
            UE_LOG(LogTemp, Log, TEXT("SAVE RESET SUCCESS: File in slot '%s' has been deleted."), *SlotName);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("SAVE RESET FAILED: File in slot '%s' exists but failed to delete."), *SlotName);
            return false;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SAVE RESET: File in slot '%s' does not exist. Proceeding with memory reset."), *SlotName);
    }

    CurrentGameData = nullptr;
    InitializeDefaultSaveData();

    return CurrentGameData != nullptr;
}

void UTestSaveGameInstance::ShowDebugData() const
{
    // 디버그 출력도 고유 슬롯 이름을 사용하도록 수정해야 합니다.
    FString SlotName = GetPlayerSpecificSaveSlotName();
    
    // ... (디버그 로직은 SlotName 변수를 사용하여 기존과 동일하게 구현하면 됩니다.)
    UE_LOG(LogTemp, Display, TEXT("=== CURRENT MEMORY DATA (Slot: %s) ==="), *SlotName);
    if (CurrentGameData)
    {
        UE_LOG(LogTemp, Display, TEXT("  > Level: %d"), CurrentGameData->Level);
        UE_LOG(LogTemp, Display, TEXT("  > Name: %s"), *CurrentGameData->PlayerName);
        UE_LOG(LogTemp, Display, TEXT("  > HP: %.2f"), CurrentGameData->HP);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("CurrentGameData is NULL (Not loaded or initialized)."));
    }
    UE_LOG(LogTemp, Display, TEXT("============================="));
}
