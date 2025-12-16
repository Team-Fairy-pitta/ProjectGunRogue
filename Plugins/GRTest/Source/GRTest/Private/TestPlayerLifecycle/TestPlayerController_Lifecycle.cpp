#include "TestPlayerLifecycle/TestPlayerController_Lifecycle.h"

#include "TestPlayerLifecycle/TestCharacter_Lifecycle.h"
#include "TestPlayerLifecycle/TestGameMode_Lifecycle.h"
#include "TestPlayerLifecycle/TestGameState_Lifecycle.h"
#include "Kismet/GameplayStatics.h"


ATestPlayerController_Lifecycle::ATestPlayerController_Lifecycle()
{
	SetReplicates(true);
}

void ATestPlayerController_Lifecycle::BeginPlay()
{
	Super::BeginPlay();
}

void ATestPlayerController_Lifecycle::Spectating()
{
	if (!HasAuthority())
	{
		ServerSpectating();
		return;
	}

	Spectating_Internal();
}

void ATestPlayerController_Lifecycle::ServerSpectating_Implementation()
{
	Spectating_Internal();
}

void ATestPlayerController_Lifecycle::Spectating_Internal()
{
	if (!HasAuthority())
	{
		return;
	}

	ChangeState(NAME_Spectating);

	ATestGameState_Lifecycle* TestGS = GetWorld() ? GetWorld()->GetGameState<ATestGameState_Lifecycle>() : nullptr;
	if (!TestGS)
	{
		return;
	}

	TestGS->UpdateAlivePlayerList();

	TArray<ATestCharacter_Lifecycle*> AlivePlayers = TestGS->GetAlivePlayers();

	CurrentIndex = 0;
}

void ATestPlayerController_Lifecycle::ServerSpectateTarget_Implementation(AActor* Target)
{
	if (!Target || !IsValid(Target))
	{
		return;
	}

	SetViewTargetWithBlend(Target, 0.0f);
}

void ATestPlayerController_Lifecycle::SpectateNext()
{
	if (!HasAuthority())
	{
		ServerSpectateNext();
		return;
	}

	SpectateNext_Internal();
}

void ATestPlayerController_Lifecycle::ServerSpectateNext_Implementation()
{
	SpectateNext_Internal();
}

void ATestPlayerController_Lifecycle::SpectateNext_Internal()
{
	if (!HasAuthority())
	{
		return;
	}

	if (GetStateName() != NAME_Spectating)
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentState = %s"), *GetStateName().ToString());
		return;
	}

	ATestGameState_Lifecycle* TestGS = GetWorld() ? GetWorld()->GetGameState<ATestGameState_Lifecycle>() : nullptr;
	if (!TestGS)
	{
		return;
	}

	TestGS->UpdateAlivePlayerList();

	TArray<ATestCharacter_Lifecycle*> AlivePlayers = TestGS->GetAlivePlayers();

	if (AlivePlayers.Num() == 0)
	{
		return;
	}

	CurrentIndex = (CurrentIndex + 1) % AlivePlayers.Num();
	if (AlivePlayers.IsValidIndex(CurrentIndex))
	{
		ServerSpectateTarget(AlivePlayers[CurrentIndex]);
	}
}

void ATestPlayerController_Lifecycle::SpectatePrevious()
{
	if (!HasAuthority())
	{
		ServerSpectatePrevious();
		return;
	}

	
	SpectatePrevious_Internal();
}

void ATestPlayerController_Lifecycle::ServerSpectatePrevious_Implementation()
{
	SpectatePrevious_Internal();
}

void ATestPlayerController_Lifecycle::SpectatePrevious_Internal()
{
	if (!HasAuthority())
	{
		return;
	}

	if (GetStateName() != NAME_Spectating)
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentState = %s"), *GetStateName().ToString());
		return;
	}

	ATestGameState_Lifecycle* TestGS = GetWorld() ? GetWorld()->GetGameState<ATestGameState_Lifecycle>() : nullptr;
	if (!TestGS)
	{
		return;
	}

	TestGS->UpdateAlivePlayerList();

	TArray<ATestCharacter_Lifecycle*> AlivePlayers = TestGS->GetAlivePlayers();

	if (AlivePlayers.Num() == 0)
	{
		return;
	}

	CurrentIndex = (CurrentIndex - 1 + AlivePlayers.Num()) % AlivePlayers.Num();
	if (AlivePlayers.IsValidIndex(CurrentIndex))
	{
		ServerSpectateTarget(AlivePlayers[CurrentIndex]);
	}
}

void ATestPlayerController_Lifecycle::Respawn()
{
	if (!HasAuthority())
	{
		ServerRespawn();
		return;
	}

	Respawn_Internal();
}

void ATestPlayerController_Lifecycle::ServerRespawn_Implementation()
{
	Respawn_Internal();
}

void ATestPlayerController_Lifecycle::Respawn_Internal()
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


