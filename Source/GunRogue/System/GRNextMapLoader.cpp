#include "System/GRNextMapLoader.h"

#include "GameModes/GRGameState.h"
#include "Components/BoxComponent.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Kismet/GameplayStatics.h"

AGRNextMapLoader::AGRNextMapLoader()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger);
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AGRNextMapLoader::BeginPlay()
{
	Super::BeginPlay();

	if (Trigger)
	{
		Trigger->OnComponentBeginOverlap.AddDynamic(this, &AGRNextMapLoader::OnOverlapBegin);
	}
}

void AGRNextMapLoader::LoadMap(TSoftObjectPtr<UWorld> LevelAsset)
{
	bool bLoadSuccessful = false;
	
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
	if (Trigger)
	{
		LoadLocation = Trigger->GetComponentLocation();
	}
	ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
		this,
		LevelAsset,
		LoadLocation,
		FRotator::ZeroRotator,
		bLoadSuccessful,
		GS->GetNextLevelName()
		);
}

void AGRNextMapLoader::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasOverlap)
	{
		return;
	}

	if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
	{
		bHasOverlap = true;
		LoadMap(LevelToLoad);
	}
}
