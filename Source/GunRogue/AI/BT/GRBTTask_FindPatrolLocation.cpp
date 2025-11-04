// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/GRBTTask_FindPatrolLocation.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName UGRBTTask_FindPatrolLocation::PatrolRandomLocationKey="PatrolRandomLocation";

UGRBTTask_FindPatrolLocation::UGRBTTask_FindPatrolLocation()
	:MinRadius(300.0f)
	,MaxRadius(1000.0f)
{
	NodeName=TEXT("Find Patrol Location");
}

EBTNodeResult::Type UGRBTTask_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* AIPawn = AICon->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	FVector Origin = AIPawn->GetActorLocation();
	
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(AIPawn->GetWorld());
	if (!NavSys) return EBTNodeResult::Failed;
	
	FVector RandomPoint;
	bool bFound = false;
	
	const int32 MaxAttempts = 3;
	for (int32 i = 0; i < MaxAttempts; ++i)
	{
		FNavLocation Tentative;
		if (NavSys->GetRandomReachablePointInRadius(Origin, MaxRadius, Tentative))
		{
			float DistSq = FVector::DistSquared(Origin, Tentative);
			if (DistSq >= FMath::Square(MinRadius))
			{
				RandomPoint = Tentative;
				bFound = true;
				break;
			}
		}
	}

	if (!bFound)
	{
		RandomPoint = Origin;
	}
	
	BlackboardComp->SetValueAsVector(PatrolRandomLocationKey, RandomPoint);

	return EBTNodeResult::Succeeded;
}
