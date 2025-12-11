#include "TestPlayerLifecycle/TestPlayerController_Lifecycle.h"

#include "TestPlayerLifecycle/TestCharacter_Lifecycle.h"
#include "TestPlayerLifecycle/TestGameMode_Lifecycle.h"
#include "Kismet/GameplayStatics.h"

void ATestPlayerController_Lifecycle::BeginPlay()
{
	Super::BeginPlay();
}

void ATestPlayerController_Lifecycle::PlayerDie()
{
	if (!HasAuthority())
	{
		ServerPlayerDie();
		return;
	}

	ATestCharacter_Lifecycle* TestCharacter = Cast<ATestCharacter_Lifecycle>(GetPawn());
	if (TestCharacter)
	{
		TestCharacter->Die();
	}
}

void ATestPlayerController_Lifecycle::ServerPlayerDie_Implementation()
{
	PlayerDie();
}

void ATestPlayerController_Lifecycle::PlayerRespawn()
{
	if (!HasAuthority())
	{
		ServerPlayerDie();
		return;
	}

	Respawn();
}

void ATestPlayerController_Lifecycle::ServerPlayerRespawn_Implementation()
{
	PlayerRespawn();
}

void ATestPlayerController_Lifecycle::Spectating()
{
	if(!HasAuthority())
	{
		return;
	}

	ChangeState(NAME_Spectating);

	// 살아있는 플레이어 리스트 갱신 
	UpdateAlivePlayerList();
	// 관전 시작
	SpectateCurrentTarget();
}

void ATestPlayerController_Lifecycle::Respawn()
{
	if (!HasAuthority())
	{
		return;
	}

	ATestGameMode_Lifecycle* GM = Cast<ATestGameMode_Lifecycle>(UGameplayStatics::GetGameMode(this));
	if (GM)
	{
		GM->RespawnPlayer(this);
	}
}

void ATestPlayerController_Lifecycle::UpdateAlivePlayerList()
{
	AlivePlayers.Empty();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATestPlayerController_Lifecycle* TestPC = Cast<ATestPlayerController_Lifecycle>(*It);
		if (TestPC && TestPC != this)
		{
			ATestCharacter_Lifecycle* TestCharacter = Cast<ATestCharacter_Lifecycle>(TestPC->GetPawn());
			if (TestCharacter && !TestCharacter->IsDead())
			{
				AlivePlayers.Add(TestCharacter);
			}
		}
	}

	// 인덱스 보정
	if (!AlivePlayers.IsValidIndex(CurrentIndex))
	{
		CurrentIndex = 0;
	}
}

void ATestPlayerController_Lifecycle::SpectateCurrentTarget()
{
	if (AlivePlayers.Num() == 0)
	{
		return;
	}

	ATestCharacter_Lifecycle* Target = AlivePlayers[CurrentIndex];
	if (Target)
	{
		SetViewTargetWithBlend(Target, 0.0f);
	}
}

void ATestPlayerController_Lifecycle::SpectateNext()
{
	if (GetStateName() != NAME_Spectating)
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentState = %s"), *GetStateName().ToString());
		return;
	}

	UpdateAlivePlayerList();
	if (AlivePlayers.Num() == 0) 
	{
		return;
	}

	CurrentIndex = (CurrentIndex + 1) % AlivePlayers.Num();
	SpectateCurrentTarget();
}

void ATestPlayerController_Lifecycle::SpectatePrevious()
{
	if (GetStateName() != NAME_Spectating)
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentState = %s"), *GetStateName().ToString());
		return;
	}

	UpdateAlivePlayerList();
	if (AlivePlayers.Num() == 0) 
	{
		return;
	}

	CurrentIndex = (CurrentIndex - 1 + AlivePlayers.Num()) % AlivePlayers.Num();
	SpectateCurrentTarget();
}
