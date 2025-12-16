#include "GameModes/Level1/GRGameMode_Level1.h"
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
