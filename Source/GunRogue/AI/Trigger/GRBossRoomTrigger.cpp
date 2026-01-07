// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Trigger/GRBossRoomTrigger.h"
#include "Character/GRCharacter.h"
#include "AI/Controller/GRBossLuwoAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

AGRBossRoomTrigger::AGRBossRoomTrigger()
{
	UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(GetRootComponent());
	if (Primitive)
	{
		Primitive->SetCollisionResponseToChannel(ECC_EngineTraceChannel3, ECR_Ignore);
	}
}

void AGRBossRoomTrigger::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &AGRBossRoomTrigger::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &AGRBossRoomTrigger::OnOverlapEnd);
}

void AGRBossRoomTrigger::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}
	
	if (!HasAuthority())
	{
		return;
	}

	if (!BossPawn)
	{
		return;
	}
	
	AGRCharacter* Player = Cast<AGRCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	AGRBossLuwoAIController* BossAI = Cast<AGRBossLuwoAIController>(BossPawn->GetController());
	if (!BossAI)
	{
		return;
	}
	
	UBlackboardComponent* BB = BossAI->GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	BossAI->AddPlayerInBossRoom(Player);
	
	int32 NumInRoom = BossAI->PlayersInBossRoomArray.Num();
	TArray<AActor*> AllPlayers;
	int32 TotalPlayers = GetWorld()->GetNumPlayerControllers();
	if (NumInRoom == TotalPlayers)
	{
		BossAI->InitBlackboardKey();
		BB->SetValueAsBool(BossAI->IsBossModeKey,true);
	}

	bool valueNow = BB->GetValueAsBool(BossAI->IsBossModeKey);
	UE_LOG(LogTemp, Warning, TEXT("Blackboard Key [%s] now = %s"), *BossAI->IsBossModeKey.ToString(), valueNow ? TEXT("true") : TEXT("false"));
}

void AGRBossRoomTrigger::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}
	
	if (!HasAuthority())
	{
		return;
	}

	if (!BossPawn)
	{
		return;
	}

	AGRCharacter* Player = Cast<AGRCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	AGRBossLuwoAIController* BossAI = Cast<AGRBossLuwoAIController>(BossPawn->GetController());
	if (!BossAI)
	{
		return;
	}
	
	BossAI->RemovePlayerInBossRoom(Player);
	
	bool bWasEmpty = BossAI->PlayersInBossRoomArray.Num() == 0;
	if (bWasEmpty)
	{
		UBlackboardComponent* BB = BossAI->GetBlackboardComponent();
		if (BB)
		{
			BB->SetValueAsBool(BossAI->IsBossModeKey,false);
		}
	}
}

