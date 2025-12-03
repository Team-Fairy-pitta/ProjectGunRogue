// Fill out your copyright notice in the Description page of Project Settings.

#include "System/GRStreamingDoorController.h"

#include "GRStreamingDoor.h"
#include "Character/GRCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


AGRStreamingDoorController::AGRStreamingDoorController()
{
	bReplicates = true;
	bIsDoorOpen = false;
	bHasLevelCompleted = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);

}


void AGRStreamingDoorController::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AGRStreamingDoorController::OnBeginOverlap);
		TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AGRStreamingDoorController::OnEndOverlap);
	}
	
}

void AGRStreamingDoorController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGRStreamingDoorController, bIsDoorOpen);
}

void AGRStreamingDoorController::OnRep_IsDoorOpen()
{
	if (bWasActivated)
	{
		return;
	}
	bWasActivated = true;
	if (TargetDoor)
	{
		TargetDoor->ActivateDoor();
	}
}

void AGRStreamingDoorController::SetLoadLevelCompleted()
{
	if (!HasAuthority())
	{
		return;
	}
	
	if (bRequiresLevelCompletion)
	{
		bHasLevelCompleted = true;
		CheckDoorOpenCondition();
	}
	UE_LOG(LogTemp, Warning, TEXT("setloadlevelcompleted"));
}

void AGRStreamingDoorController::CheckDoorOpenCondition()
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
	
	bool bShouldOpen = (CurrentPlayers >= TotalPlayers) && (TotalPlayers > 0);

	if (bRequiresLevelCompletion && !bHasLevelCompleted)
	{
		UE_LOG(LogTemp, Warning, TEXT("Back"));
		return;
	}

	if (bIsDoorOpen != bShouldOpen)
	{
		bIsDoorOpen = bShouldOpen;

		OnRep_IsDoorOpen();
	}
	UE_LOG(LogTemp, Warning, TEXT("TotalPlayer: %d"), TotalPlayers);
	UE_LOG(LogTemp, Warning, TEXT("PlayersInArea: %d"), CurrentPlayers);
	UE_LOG(LogTemp, Warning, TEXT("Current bisDoorOpen : %s"), bIsDoorOpen ? TEXT("OPEN") : TEXT("CLOSED"));
}

void AGRStreamingDoorController::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
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
		CheckDoorOpenCondition();
	}
}

void AGRStreamingDoorController::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
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

