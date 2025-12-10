#include "GameModes/Level1/GRGameMode_Level1.h"
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
