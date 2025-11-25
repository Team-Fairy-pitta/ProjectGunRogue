// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/GRBTTask_CheckAttackRangeState.h"

#include "AI/GRAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/GRBossLuwoAIController.h"

UGRBTTask_CheckAttackRangeState::UGRBTTask_CheckAttackRangeState()
	:CloseRange(500.0f)
	,MidRange(1000.0f)
{
	NodeName = TEXT("Check Attack Range State");
}

EBTNodeResult::Type UGRBTTask_CheckAttackRangeState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
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

	FVector TargetLocation = TargetActor->GetActorLocation();

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

	FVector AILocation = AIPawn->GetActorLocation();
	float DistSq = FVector::DistSquared(TargetLocation,AILocation);

	EBossAttackRangeState FoundRangeState = EBossAttackRangeState::None;

	if (DistSq <= CloseRange * CloseRange)
	{
		FoundRangeState = EBossAttackRangeState::Close;
	}
	else if (DistSq <= MidRange * MidRange)
	{
		FoundRangeState = EBossAttackRangeState::Middle;
	}
	else
	{
		FoundRangeState = EBossAttackRangeState::Far;

		int32 RandIndex = FMath::RandRange(0,1);
		BB->SetValueAsInt(AGRBossLuwoAIController::FarAttackRandomIndexKey,RandIndex);
	}

	BB->SetValueAsEnum(AGRBossLuwoAIController::BossAttackRangeStateKey, static_cast<uint8>(FoundRangeState));

	//NOTE : Debug Draw
#if WITH_EDITOR
	UWorld* World = AIPawn->GetWorld();
	if (World)
	{
		DrawDebugSphere(World, AILocation, CloseRange, 12, FColor::Blue, false, 1.0f);
		DrawDebugSphere(World, AILocation, MidRange, 12, FColor::Green, false, 1.0f);
	}
#endif
	
	return EBTNodeResult::Succeeded;
}
