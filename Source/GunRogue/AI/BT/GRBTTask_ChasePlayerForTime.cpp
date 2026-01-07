// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/GRBTTask_ChasePlayerForTime.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Controller/GRBossLuwoAIController.h"
#include "Navigation/PathFollowingComponent.h"

UGRBTTask_ChasePlayerForTime::UGRBTTask_ChasePlayerForTime()
	:ChaseDuration(3.0f)
	,AcceptanceRadius(100.0f)
{
	bNotifyTick = true;

	NodeName=TEXT("Chase Player For Time");
}

uint16 UGRBTTask_ChasePlayerForTime::GetInstanceMemorySize() const
{
	return sizeof(FChaseMemory);
}

EBTNodeResult::Type UGRBTTask_ChasePlayerForTime::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FChaseMemory* MyMemory = reinterpret_cast<FChaseMemory*>(NodeMemory);
	MyMemory->TimeElapsed = 0.f;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}

	UObject* TargetObj = BB->GetValueAsObject(AGRBossLuwoAIController::TargetPlayerKey);
	if (!TargetObj)
	{
		return EBTNodeResult::Failed;
	}
	
	AActor* TargetActor = Cast<AActor>(TargetObj);
	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}
	
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon)
	{
		return EBTNodeResult::Failed;
	}
	
	APawn* AIPawn =AICon->GetPawn();
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}
	
	FAIMoveRequest MoveReq(TargetActor);
	MoveReq.SetAcceptanceRadius(AcceptanceRadius);
	FNavPathSharedPtr NavPath;
	AICon->MoveTo(MoveReq, &NavPath);

	return EBTNodeResult::InProgress;
}

void UGRBTTask_ChasePlayerForTime::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	FChaseMemory* MyMemory = reinterpret_cast<FChaseMemory*>(NodeMemory);
	MyMemory->TimeElapsed += DeltaSeconds;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	//TODO : 추후 타겟 플레이어가 죽으면 TargetPlayerKey 바꿔주기
	
	UObject* TargetObj = BB->GetValueAsObject(AGRBossLuwoAIController::TargetPlayerKey);
	if (!TargetObj)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	AActor* TargetActor = Cast<AActor>(TargetObj);
	if (!TargetActor)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UPathFollowingComponent* PFC = AICon->GetPathFollowingComponent();
	if (!IsValid(PFC))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	// EPathFollowingStatus::Type Status = PFC->GetStatus();
	// if (Status == EPathFollowingStatus::Idle)
	// {
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// 	return;
	// }

	if (MyMemory->TimeElapsed >= ChaseDuration)
	{
		AICon->StopMovement();
		
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
}


