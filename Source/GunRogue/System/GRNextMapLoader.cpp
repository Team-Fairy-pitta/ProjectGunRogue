#include "System/GRNextMapLoader.h"

#include "GRStreamingDoor.h"
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
		Trigger->OnComponentBeginOverlap.AddDynamic(this, &AGRNextMapLoader::OnOverlapBegin);
		Trigger->OnComponentEndOverlap.AddDynamic(this, &AGRNextMapLoader::OnOverlapEnd);
	}
}

void AGRNextMapLoader::LoadMap(TSoftObjectPtr<UWorld> LevelAsset)
{
	if (bHasOverlap)
	{
		return;
	}
	bool bLoadSuccessful = false;
	bHasOverlap = true;
	
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

	if (StreamedLevel && TargetDoor)
	{
		StreamedLevel->OnLevelLoaded.AddDynamic(this, &AGRNextMapLoader::OnLevelLoadCompleted);
	}
	
}

void AGRNextMapLoader::OnLevelLoadCompleted()
{
	if (TargetDoor)
	{
		TargetDoor->ActivateDoor();
	}
	UE_LOG(LogTemp, Warning, TEXT("CallbackDebug On"));
}

void AGRNextMapLoader::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (bHasOverlap)
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

void AGRNextMapLoader::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority()) 
	{
		return;
	}
	
	if (bHasOverlap)
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

	DOREPLIFETIME(AGRNextMapLoader, bIsStream);
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

	if (bIsStream != bShouldLoad)
	{
		bIsStream = bShouldLoad;

		OnRep_IsStream();
	}
	UE_LOG(LogTemp, Warning, TEXT("TotalPlayer: %d"), TotalPlayers);
	UE_LOG(LogTemp, Warning, TEXT("PlayersInArea: %d"), CurrentPlayers);
	UE_LOG(LogTemp, Warning, TEXT("Current bIsStream : %s"), bIsStream ? TEXT("OPEN") : TEXT("CLOSED"));
}

void AGRNextMapLoader::OnRep_IsStream()
{
	LoadMap(LevelToLoad);
}
