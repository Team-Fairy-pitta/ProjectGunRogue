#include "Player/Battle/GRBattleCheatManager.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "GameModes/Level1/GRGameMode_Level1.h"
#include "GameModes/Level1/GRGameState_Level1.h"
#include "AI/Character/GRAICharacter.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "Kismet/GameplayStatics.h"

void UGRBattleCheatManager::SetLevel1NextRoomIndex(int32 InIndex)
{
	APlayerController* PC = GetOuterAPlayerController();
	if (!IsValid(PC))
	{
		return;
	}

	if (!PC->HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("Cheat SetLevel1NextRoomIndex requires Authority"));
		return;
	}

	if (!PC->GetWorld())
	{
		return;
	}

	AGRGameMode_Level1* GameMode_Level1 = PC->GetWorld()->GetAuthGameMode<AGRGameMode_Level1>();
	if (!IsValid(GameMode_Level1))
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode is NOT AGRGameMode_Level1"));
		return;
	}

	AGRGameState_Level1* GameState_Level1 = PC->GetWorld()->GetGameState<AGRGameState_Level1>();
	if (!IsValid(GameState_Level1))
	{
		UE_LOG(LogTemp, Error, TEXT("GameState is NOT AGRGameState_Level1"));
		return;
	}

	if (!GameMode_Level1->RandomLevelPool.IsValidIndex(InIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("InIndex is INVALID"));
		return;
	}

	int32 MAX = 6 * 6; // 맵 크기

	for (int32 Index = 0; Index <= MAX; ++Index)
	{
		FGRLevel1Node* Node = GameMode_Level1->GetLevel1Node(Index);
		if (Node)
		{
			Node->LevelToLoad = GameMode_Level1->RandomLevelPool[InIndex];
		}
	}

	GameState_Level1->RequestNextRoomInformation();
}

void UGRBattleCheatManager::SetLevel1NextRoomBoss()
{
	APlayerController* PC = GetOuterAPlayerController();
	if (!IsValid(PC))
	{
		return;
	}

	if (!PC->HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("Cheat SetLevel1NextRoomIndex requires Authority"));
		return;
	}

	if (!PC->GetWorld())
	{
		return;
	}

	AGRGameMode_Level1* GameMode_Level1 = PC->GetWorld()->GetAuthGameMode<AGRGameMode_Level1>();
	if (!IsValid(GameMode_Level1))
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode is NOT AGRGameMode_Level1"));
		return;
	}

	AGRGameState_Level1* GameState_Level1 = PC->GetWorld()->GetGameState<AGRGameState_Level1>();
	if (!IsValid(GameState_Level1))
	{
		UE_LOG(LogTemp, Error, TEXT("GameState is NOT AGRGameState_Level1"));
		return;
	}


	int32 MAX = 6 * 6; // 맵 크기

	for (int32 Index = 0; Index <= MAX; ++Index)
	{
		FGRLevel1Node* Node = GameMode_Level1->GetLevel1Node(Index);
		if (Node)
		{
			Node->LevelToLoad = GameMode_Level1->BossLevel;
		}
	}

	GameState_Level1->RequestNextRoomInformation();
}

void UGRBattleCheatManager::GetPlayerName(int32 InIndex)
{
	APlayerController* PC = GetOuterAPlayerController();
	if (!IsValid(PC))
	{
		return;
	}

	if (!PC->HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("Cheat SetLevel1NextRoomIndex requires Authority"));
		return;
	}

	AGRGameState_Level1* GameState_Level1 = PC->GetWorld()->GetGameState<AGRGameState_Level1>();
	if (!IsValid(GameState_Level1))
	{
		UE_LOG(LogTemp, Error, TEXT("GameState is NOT AGRGameState_Level1"));
		return;
	}

	if (!GameState_Level1->PlayerArray.IsValidIndex(InIndex))
	{
		return;
	}

	APlayerState* PlayerState = GameState_Level1->PlayerArray[InIndex];
	AGRPlayerState* GRPlayerState = Cast<AGRPlayerState>(PlayerState);
	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerState is NOT AGRPlayerState"));
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("PlayerName: %s"), *GRPlayerState->GetPlayerName());
}

void UGRBattleCheatManager::KillPlayer(int32 InIndex)
{
	APlayerController* PC = GetOuterAPlayerController();
	if (!IsValid(PC))
	{
		return;
	}

	if (!PC->HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("Cheat SetLevel1NextRoomIndex requires Authority"));
		return;
	}

	AGRGameState_Level1* GameState_Level1 = PC->GetWorld()->GetGameState<AGRGameState_Level1>();
	if (!IsValid(GameState_Level1))
	{
		UE_LOG(LogTemp, Error, TEXT("GameState is NOT AGRGameState_Level1"));
		return;
	}

	if (!GameState_Level1->PlayerArray.IsValidIndex(InIndex))
	{
		return;
	}

	APlayerState* PlayerState = GameState_Level1->PlayerArray[InIndex];
	AGRPlayerState* GRPlayerState = Cast<AGRPlayerState>(PlayerState);
	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerState is NOT AGRPlayerState"));
		return;
	}

	UAbilitySystemComponent* ASC = GRPlayerState->GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	ASC->SetNumericAttributeBase(UGRHealthAttributeSet::GetHealthAttribute(), 0.0f);
}

void UGRBattleCheatManager::RespawnPlayer(int32 InIndex)
{
	APlayerController* PC = GetOuterAPlayerController();
	if (!IsValid(PC))
	{
		return;
	}

	if (!PC->HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("Cheat SetLevel1NextRoomIndex requires Authority"));
		return;
	}

	AGRGameState_Level1* GameState_Level1 = PC->GetWorld()->GetGameState<AGRGameState_Level1>();
	if (!IsValid(GameState_Level1))
	{
		UE_LOG(LogTemp, Error, TEXT("GameState is NOT AGRGameState_Level1"));
		return;
	}

	if (!GameState_Level1->PlayerArray.IsValidIndex(InIndex))
	{
		return;
	}

	APlayerState* PlayerState = GameState_Level1->PlayerArray[InIndex];
	AGRPlayerState* GRPlayerState = Cast<AGRPlayerState>(PlayerState);
	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerState is NOT AGRPlayerState"));
		return;
	}

	if (!PC->GetWorld())
	{
		return;
	}

	AGRGameMode_Level1* GameMode_Level1 = PC->GetWorld()->GetAuthGameMode<AGRGameMode_Level1>();
	if (!IsValid(GameMode_Level1))
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode is NOT AGRGameMode_Level1"));
		return;
	}

	APlayerController* TargetPlayerController = GRPlayerState->GetPlayerController();
	if (!IsValid(TargetPlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("TargetPlayerController is INVALID"));
		return;
	}

	AGRBattlePlayerController* BPC = Cast<AGRBattlePlayerController>(TargetPlayerController);

	auto AlivePlayers = BPC->GetAlivePlayerList();

	if (AlivePlayers.Num() > 0)
	{
		GameMode_Level1->TryRespawnPlayer(TargetPlayerController, AlivePlayers[0]);
	}
}

void UGRBattleCheatManager::RespawnAllDeadPlayer()
{
	APlayerController* PC = GetOuterAPlayerController();
	if (!IsValid(PC))
	{
		return;
	}

	if (!PC->HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("Cheat SetLevel1NextRoomIndex requires Authority"));
		return;
	}

	if (!PC->GetWorld())
	{
		return;
	}

	AGRGameMode_Level1* GameMode_Level1 = PC->GetWorld()->GetAuthGameMode<AGRGameMode_Level1>();
	if (!IsValid(GameMode_Level1))
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode is NOT AGRGameMode_Level1"));
		return;
	}
	GameMode_Level1->TryRespawnAllDeadPlayers();
}

void UGRBattleCheatManager::KillAllAI()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGRAICharacter::StaticClass(), FoundActors);

	// 사용 예시
	for (AActor* Actor : FoundActors)
	{
		if (AGRAICharacter* AICharacter = Cast<AGRAICharacter>(Actor))
		{
			auto* ASC = AICharacter->GetAbilitySystemComponent();
			if (ASC)
			{
				ASC->SetNumericAttributeBase(UGRHealthAttributeSet::GetHealthAttribute(), 0.0f);
			}
		}
	}
}

void UGRBattleCheatManager::BroadcastMessage(FString Message, float ShowMessageTime)
{
	APlayerController* PC = GetOuterAPlayerController();
	if (!IsValid(PC))
	{
		return;
	}
	if (!PC->HasAuthority())
	{
		return;
	}
	if (!PC->GetWorld())
	{
		return;
	}

	AGRGameMode_Level1* GameMode_Level1 = PC->GetWorld()->GetAuthGameMode<AGRGameMode_Level1>();
	if (!IsValid(GameMode_Level1))
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode is NOT AGRGameMode_Level1"));
		return;
	}
	GameMode_Level1->BroadcastNotifyMessage(FText::FromString(Message), ShowMessageTime);
}
