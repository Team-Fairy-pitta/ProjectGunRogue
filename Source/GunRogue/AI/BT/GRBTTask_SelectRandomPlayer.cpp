// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/GRBTTask_SelectRandomPlayer.h"
#include "AI/Controller/GRBossLuwoAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GRCharacter.h"

UGRBTTask_SelectRandomPlayer::UGRBTTask_SelectRandomPlayer()
{
	NodeName = "Select Random Player";
}

EBTNodeResult::Type UGRBTTask_SelectRandomPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AGRBossLuwoAIController* BossAI = Cast<AGRBossLuwoAIController>(OwnerComp.GetAIOwner());
	if (!BossAI)
	{
		return EBTNodeResult::Failed;
	}
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}

	UWorld* World = OwnerComp.GetWorld();
	if (!World)
	{
		return EBTNodeResult::Failed;
	}
	
	if ( BossAI->PlayersInBossRoomArray.Num() == 0)
	{
		return EBTNodeResult::Failed;
	}

	TArray<AActor*> ValidPlayers;
	for (auto& WeakPtr : BossAI->PlayersInBossRoomArray)
	{
		AActor* Actor = WeakPtr.Get();
		if (!Actor)
		{
			continue;
		}
		
		AGRCharacter* PlayerCharacter = Cast<AGRCharacter>(Actor);
		if (!PlayerCharacter)
		{
			continue;
		}

		//TODO : 추후 플레이어가 죽었을 경우는 예외처리
		
		ValidPlayers.Add(PlayerCharacter);
	}

	if (ValidPlayers.Num() == 0)
	{
		return EBTNodeResult::Failed;
	}

	int32 RandomIndex=FMath::RandRange(0,ValidPlayers.Num()-1);
	AActor* RandomPlayer = ValidPlayers[RandomIndex];

	BB->SetValueAsObject(AGRBossLuwoAIController::TargetPlayerKey, RandomPlayer);
	
	return EBTNodeResult::Succeeded;
}
