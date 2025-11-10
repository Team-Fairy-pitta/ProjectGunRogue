#include "System/GRPreviousMapUnloader.h"

#include "Kismet/GameplayStatics.h"
#include "GameModes/GRGameState.h"
#include "Components/BoxComponent.h"


AGRPreviousMapUnloader::AGRPreviousMapUnloader()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger);
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AGRPreviousMapUnloader::BeginPlay()
{
	Super::BeginPlay();

	if (Trigger)
	{
		Trigger->OnComponentBeginOverlap.AddDynamic(this, &AGRPreviousMapUnloader::OnOverlapBegin);
	}
}

void AGRPreviousMapUnloader::MapUnload()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FLatentActionInfo LatentInfo;
	LatentInfo.UUID = FMath::RandRange(1000,9999);
	LatentInfo.Linkage = 0;
	LatentInfo.CallbackTarget = this;
	
	AGameStateBase* CurrentGameState = UGameplayStatics::GetGameState(GetWorld());
	if (!CurrentGameState)
	{
		return;
	}
		
	AGRGameState* GS = Cast<AGRGameState>(CurrentGameState);
	if (GS)
	{
		int32 UnloadLevelNumber = GS->GetCurrentLevel() - 2;
		FString LevelNameString = FString::Printf(TEXT("LevelInst_%d"), UnloadLevelNumber);
		UGameplayStatics::UnloadStreamLevel(
			World,
			FName(*LevelNameString),
			LatentInfo,
			false);
	}
	
	

}

void AGRPreviousMapUnloader::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasOverlap)
	{
		return;
	}

	if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
	{
		bHasOverlap = true;
		MapUnload();
	}
}
