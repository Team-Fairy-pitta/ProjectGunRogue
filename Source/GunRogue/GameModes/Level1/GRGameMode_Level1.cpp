#include "GameModes/Level1/GRGameMode_Level1.h"
#include "GameModes/Level1/GRGameState_Level1.h"
#include "Character/GRCharacter.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"
#include "System/GRLevel1ControlPanel.h"
#include "System/GRGameInstance.h"

void AGRGameMode_Level1::BeginPlay()
{
	Super::BeginPlay();

	Level1Data.InitAtServer(this);

#if WITH_EDITOR
	Level1Data.PrintDebugLog();
#endif
}

UClass* AGRGameMode_Level1::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	UGRGameInstance* GRGameInstance = GetGameInstance<UGRGameInstance>();
	if (!GRGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("GRGameInstance is INVALID"));
		return nullptr;
	}

	APlayerController* PlayerController = Cast<APlayerController>(InController);
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is INVALID"));
		return nullptr;
	}

	return GRGameInstance->GetSelectedCharacterClass(PlayerController);
}

bool AGRGameMode_Level1::TryRespawnPlayer(AController* TargetPlayer, AActor* AlivePlayer)
{
	AGRBattlePlayerController* BattlePlayerController = Cast<AGRBattlePlayerController>(TargetPlayer);
	if (!IsValid(BattlePlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("TargetPlayer is NOT AGRBattlePlayerController"));
		return false;
	}

	AGRPlayerState* GRPlayerState = TargetPlayer->GetPlayerState<AGRPlayerState>();
	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerState is INVALID"));
		return false;
	}

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(FindSpawnableLocation(AlivePlayer));

	RestartPlayerAtTransform(TargetPlayer, SpawnTransform);

	APawn* RestartedPawn = TargetPlayer->GetPawn();
	if (!IsValid(RestartedPawn))
	{
		UE_LOG(LogTemp, Error, TEXT("Fail: RespawnPlayer"));
		return false;
	}
	else
	{
		GRPlayerState->RestoreHealthAndShield();
		BattlePlayerController->ClientRPC_OnRestartPlayer();
		return true;
	}
}

void AGRGameMode_Level1::TryRespawnAllDeadPlayers()
{
	TArray<AGRPlayerState*> AlivePlayerStates;
	TArray<AGRPlayerState*> DeadPlayerStates;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AGRBattlePlayerController* BattlePlayerController = Cast<AGRBattlePlayerController>(*It);
		if (IsValid(BattlePlayerController))
		{
			AGRPlayerState* GRPlayerState = BattlePlayerController->GetPlayerState<AGRPlayerState>();
			if (IsValid(GRPlayerState))
			{
				if (GRPlayerState->IsDead())
				{
					UE_LOG(LogTemp, Log, TEXT("TryRespawnAllDeadPlayers: Add DeadPlayerStates %s"), *GRPlayerState->GetPlayerName());
					DeadPlayerStates.Add(GRPlayerState);
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("TryRespawnAllDeadPlayers: Add AlivePlayerStates %s"), *GRPlayerState->GetPlayerName());
					AlivePlayerStates.Add(GRPlayerState);
				}
			}
		}
	}

	if (AlivePlayerStates.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("TryRespawnAllDeadPlayers: AlivePlayerStates.Num() <= 0"));
		return;
	}
	

	int32 RespawnedPlayerCount = 0;
	for (AGRPlayerState* PlayerState : DeadPlayerStates)
	{
		AGRCharacter* Body = PlayerState->GetPawn<AGRCharacter>();
		if (IsValid(Body))
		{
			continue;
		}

		APlayerController* PlayerController = PlayerState->GetPlayerController();
		AActor* AlivePlayerActor = AlivePlayerStates[0]->GetPawn();
		bool bRespawnSuccess = TryRespawnPlayer(PlayerController, AlivePlayerActor);
		if (bRespawnSuccess)
		{
			RespawnedPlayerCount += 1;
		}
	}

	if (RespawnedPlayerCount < DeadPlayerStates.Num())
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::TryRespawnAllDeadPlayers);
	}
}

FGRLevel1Node* AGRGameMode_Level1::GetLevel1Node(int32 Index)
{
	if (Level1Data.IsValidData())
	{
		return Level1Data.GetNode(Index);
	}
	else
	{
		return nullptr;
	}
}

void AGRGameMode_Level1::ReceiveSpawnEnemy()
{
	EnemyCount += 1;
	UpdateLevel1ControlPanel();
}

void AGRGameMode_Level1::ReceiveDestroyEnemy()
{
	EnemyCount = EnemyCount - 1 >= 0 ? EnemyCount - 1 : 0;
	UpdateLevel1ControlPanel();

	if (EnemyCount <= 0)
	{
		TryRespawnAllDeadPlayers();
		GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::BroadcastClearMessage);
	}
}

bool AGRGameMode_Level1::CheckGameOver()
{
	AGameStateBase* GameStateBsae = GetWorld()->GetGameState<AGameStateBase>();
	if (!IsValid(GameStateBsae))
	{
		return false;
	}

	int32 AlivePlayerCount = 0;
	for (APlayerState* ItState : GameStateBsae->PlayerArray)
	{
		AGRPlayerState* GRPlayerState = Cast<AGRPlayerState>(ItState);
		if (IsValid(GRPlayerState) && !GRPlayerState->IsDead())
		{
			AlivePlayerCount += 1;
		}
	}

	if (AlivePlayerCount <= 0)
	{
		GameOver();
		return true;
	}
	else
	{
		return false;
	}
}

void AGRGameMode_Level1::GameOver()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AGRBattlePlayerController* BattlePlayerController = Cast<AGRBattlePlayerController>(*It);
		if (IsValid(BattlePlayerController))
		{
			BattlePlayerController->ClientRPC_GameOver();
		}
	}
}

void AGRGameMode_Level1::AddLevel1ControlPanel(AGRLevel1ControlPanel* Level1ControlPanel)
{
	Panels.Add(Level1ControlPanel);
	UpdateLevel1ControlPanel();
}

void AGRGameMode_Level1::RemoveLevel1ControlPanel(AGRLevel1ControlPanel* Level1ControlPanel)
{
	Panels.Remove(Level1ControlPanel);
	UpdateLevel1ControlPanel();
}

void AGRGameMode_Level1::BroadcastNotifyMessage(const FText& Message, float ShowMessageTime)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AGRBattlePlayerController* PC = Cast<AGRBattlePlayerController>(It->Get());

		if (PC)
		{
			PC->ClientRPC_ShowNotifyMessage(Message,ShowMessageTime);
		}
	}
}

void AGRGameMode_Level1::UpdateLevel1ControlPanel()
{
	bool bHasEliminatedEnemies;
	if (EnemyCount <= 0)
	{
		bHasEliminatedEnemies = true;
	}
	else
	{
		bHasEliminatedEnemies = false;
	}

	for (auto Panel : Panels)
	{
		Panel->SetbHasEliminatedEnemies(bHasEliminatedEnemies);
	}
}

FVector AGRGameMode_Level1::FindSpawnableLocation(AActor* AlivePlayer)
{
	// TODO
	if (!IsValid(AlivePlayer))
	{
		return FVector::ZeroVector;
	}

	return AlivePlayer->GetActorLocation();
}

void AGRGameMode_Level1::BroadcastClearMessage()
{
	FString MessageString(TEXT("문 개폐장치가 활성화 되었습니다."));
	FText Message = FText::FromString(MessageString);
	float Duration = 4.0f;
	BroadcastNotifyMessage(Message, Duration);
}

void AGRGameMode_Level1::BroadcastOpenNextStage()
{
	FString MessageString(TEXT("다음 스테이지가 선택되었습니다."));
	FText Message = FText::FromString(MessageString);
	float Duration = 4.0f;
	BroadcastNotifyMessage(Message, Duration);
}
