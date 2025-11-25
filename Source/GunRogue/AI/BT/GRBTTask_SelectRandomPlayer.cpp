// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/GRBTTask_SelectRandomPlayer.h"

#include "AI/GRBossLuwoAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GRCharacter.h"

UGRBTTask_SelectRandomPlayer::UGRBTTask_SelectRandomPlayer()
{
	NodeName = "Select Random Player";
}

EBTNodeResult::Type UGRBTTask_SelectRandomPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
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

	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(World, AGRCharacter::StaticClass(), Players);

	if (Players.Num() == 0)
	{
		return EBTNodeResult::Failed;
	}

	TArray<AActor*> ValidPlayers;
	for (AActor* Player : Players)
	{
		AGRCharacter* PlayerCharacter = Cast<AGRCharacter>(Player);
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
