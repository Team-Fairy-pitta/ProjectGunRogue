#include "Player/GRPlayerController.h"
#include "Player/GRPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/GRGameplayEffect.h"

void AGRPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	AGRPlayerState* GRPlayerState = GetPlayerState<AGRPlayerState>();
	if (!IsValid(GRPlayerState))
	{
		return;
	}

	UGRAbilitySystemComponent* ASC = GRPlayerState->GetGRAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}
	ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
}

AActor* AGRPlayerController::GetPreviousSpectateActor()
{
	TArray<AActor*> AlivePlayerList = GetAlivePlayerList();
	if (AlivePlayerList.IsValidIndex(CurrentSpectateIndex))
	{
		CurrentSpectateIndex = CurrentSpectateIndex - 1 < 0 ? AlivePlayerList.Num() - 1 : CurrentSpectateIndex - 1;
		return AlivePlayerList[CurrentSpectateIndex];
	}
	else
	{
		if (AlivePlayerList.Num() > 0)
		{
			CurrentSpectateIndex = 0;
			return AlivePlayerList[CurrentSpectateIndex];
		}
		else
		{
			CurrentSpectateIndex = INDEX_NONE;
			return nullptr;
		}
	}
}

AActor* AGRPlayerController::GetNextSpectateActor()
{
	TArray<AActor*> AlivePlayerList = GetAlivePlayerList();
	if (AlivePlayerList.IsValidIndex(CurrentSpectateIndex))
	{
		CurrentSpectateIndex = CurrentSpectateIndex + 1 >= AlivePlayerList.Num() ? 0 : CurrentSpectateIndex + 1;
		return AlivePlayerList[CurrentSpectateIndex];
	}
	else
	{
		if (AlivePlayerList.Num() > 0)
		{
			CurrentSpectateIndex = 0;
			return AlivePlayerList[CurrentSpectateIndex];
		}
		else
		{
			CurrentSpectateIndex = INDEX_NONE;
			return nullptr;
		}
	}
}

TArray<AActor*> AGRPlayerController::GetAlivePlayerList()
{
	if (!GetWorld())
	{
		return TArray<AActor*>();
	}

	AGameStateBase* GameStateBsae = GetWorld()->GetGameState<AGameStateBase>();
	if (!IsValid(GameStateBsae))
	{
		return TArray<AActor*>();
	}

	TArray<AActor*> PlayerList;
	
	for (APlayerState* ItState : GameStateBsae->PlayerArray)
	{
		AGRPlayerState* GRPlayerState = Cast<AGRPlayerState>(ItState);
		if (IsValid(GRPlayerState) && !GRPlayerState->IsDead())
		{
			APawn* ItPawn = GRPlayerState->GetPawn();
			if (IsValid(ItPawn))
			{
				PlayerList.Add(ItPawn);
			}
		}
	}

	return PlayerList;
}

void AGRPlayerController::ServerRPC_SpectatePreviousPlayer_Implementation()
{
	AActor* TargetActor = GetPreviousSpectateActor();
	if (IsValid(TargetActor))
	{
		ChangeState(NAME_Spectating);
		SetViewTargetWithBlend(TargetActor);
	}
	else
	{
		ChangeState(NAME_Playing);
	}
}

void AGRPlayerController::ServerRPC_SpectateNextPlayer_Implementation()
{
	AActor* TargetActor = GetNextSpectateActor();
	if (IsValid(TargetActor))
	{
		ChangeState(NAME_Spectating);
		SetViewTargetWithBlend(TargetActor);
	}
	else
	{
		ChangeState(NAME_Playing);
	}
}

void AGRPlayerController::ServerRPC_StartSpectating_Implementation()
{
	if (!HasAuthority())
	{
		return;
	}

	ChangeState(NAME_Spectating);
	ServerRPC_SpectateNextPlayer();
}
