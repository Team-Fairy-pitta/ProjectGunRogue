#include "System/GRNextMapLoader.h"

#include "GRStreamingDoorController.h"
#include "Components/ArrowComponent.h"
#include "GameModes/GRGameState.h"
#include "Components/BoxComponent.h"
#include "Engine/LevelStreamingDynamic.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AGRNextMapLoader::AGRNextMapLoader()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bShouldLoadLevel = false;
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger);
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(Trigger);
}

void AGRNextMapLoader::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		Trigger->OnComponentBeginOverlap.AddDynamic(this, &AGRNextMapLoader::OnBeginOverlap);
		Trigger->OnComponentEndOverlap.AddDynamic(this, &AGRNextMapLoader::OnEndOverlap);
	}
}

void AGRNextMapLoader::LoadMap(TSoftObjectPtr<UWorld> LevelAsset)
{
	if (bWasActivated)
	{
		return;
	}
	bool bLoadSuccessful = false;
	bWasActivated = true;
	
	AGameStateBase* CurrentGameState = UGameplayStatics::GetGameState(GetWorld());
	if (!CurrentGameState)
	{
		return;
	}
		
	AGRGameState* GS = Cast<AGRGameState>(CurrentGameState);
	if (!GS)
	{
		return;
	}
	
	FVector LoadLocation = FVector::ZeroVector;
	if (Arrow)
	{
		LoadLocation = Arrow->GetComponentLocation();
	}
	
	ULevelStreamingDynamic* StreamedLevel = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
		this,
		LevelAsset,
		LoadLocation,
		FRotator::ZeroRotator,
		bLoadSuccessful,
		GS->GetNextLevelName()
		);

	if (StreamedLevel && TargetController)
	{
		StreamedLevel->OnLevelLoaded.AddDynamic(this, &AGRNextMapLoader::OnLevelLoadCompleted);
	}
	
}

void AGRNextMapLoader::OnLevelLoadCompleted()
{
	if (TargetController)
	{
		TargetController->SetLoadLevelCompleted();
	}
	UE_LOG(LogTemp, Warning, TEXT("CallbackDebug On"));
}

void AGRNextMapLoader::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (bWasActivated)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character && Character->GetPlayerState())
	{
		PlayersInArea.Add(Character->GetPlayerState());
		CheckMapLoaderCondition();
	}
}

void AGRNextMapLoader::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority()) 
	{
		return;
	}
	
	if (bWasActivated)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character && Character->GetPlayerState())
	{
		PlayersInArea.Remove(Character->GetPlayerState()); 
	}
}

void AGRNextMapLoader::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGRNextMapLoader, bShouldLoadLevel);
}

void AGRNextMapLoader::CheckMapLoaderCondition()
{
	if (!HasAuthority())
	{
		return;
	}

	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());
	if (!GameMode)
	{
		return;
	}

	int32 TotalPlayers = GameMode->GetNumPlayers();

	int32 CurrentPlayers = PlayersInArea.Num();
	
	bool bShouldLoad = (CurrentPlayers >= TotalPlayers) && (TotalPlayers > 0);

	if (bShouldLoadLevel != bShouldLoad)
	{
		bShouldLoadLevel = bShouldLoad;

		OnRep_ShouldLoadLevel();
	}
	UE_LOG(LogTemp, Warning, TEXT("TotalPlayer: %d"), TotalPlayers);
	UE_LOG(LogTemp, Warning, TEXT("PlayersInArea: %d"), CurrentPlayers);
	UE_LOG(LogTemp, Warning, TEXT("Current bIsStream : %s"), bShouldLoadLevel ? TEXT("OPEN") : TEXT("CLOSED"));
}

void AGRNextMapLoader::OnRep_ShouldLoadLevel()
{
	if (LevelToLoad.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("return Leveltoload isvalid"));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("bshoudloadlevel: %d"), bShouldLoadLevel);
	if (bShouldLoadLevel)
	{
		LoadMap(LevelToLoad);
	}
}
