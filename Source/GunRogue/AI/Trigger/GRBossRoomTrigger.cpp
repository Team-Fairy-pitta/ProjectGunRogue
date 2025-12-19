// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Trigger/GRBossRoomTrigger.h"
#include "Character/GRCharacter.h"
#include "AI/Controller/GRBossLuwoAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

AGRBossRoomTrigger::AGRBossRoomTrigger()
{
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
	
	bool bWasEmpty = BossAI->PlayersInBossRoomArray.Num() == 0;
	if (bWasEmpty)
	{
		UBlackboardComponent* BB = BossAI->GetBlackboardComponent();
		if (BB)
		{
			BB->SetValueAsBool(BossAI->IsBossModeKey,true);
		}
	}
		
	BossAI->AddPlayerInBossRoom(Player);
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
	
	bool bWasEmpty = BossAI->PlayersInBossRoomArray.Num() == 0;
	if (bWasEmpty)
	{
		UBlackboardComponent* BB = BossAI->GetBlackboardComponent();
		if (BB)
		{
			BB->SetValueAsBool(BossAI->IsBossModeKey,false);
		}
	}

	BossAI->RemovePlayerInBossRoom(Player);
}

