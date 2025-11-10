// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/GRBTTask_SwapPatrolPointsIfClose.h"
#include "AI/GRAIController.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/BT/GRBTTask_FindRandomLocation.h"

UGRBTTask_SwapPatrolPointsIfClose::UGRBTTask_SwapPatrolPointsIfClose()
{
	NodeName = TEXT("Swap Patrol Points If Close");
}

EBTNodeResult::Type UGRBTTask_SwapPatrolPointsIfClose::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon)
	{
		return EBTNodeResult::Failed;
	}

	APawn* AIPawn = AICon->GetPawn();
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	FVector AILocation = AIPawn->GetActorLocation();

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(AIPawn->GetWorld());
	if (!NavSys)
	{
		return EBTNodeResult::Failed;
	}
	
	FVector PatrolStartLocation = BlackboardComp->GetValueAsVector(AGRAIController::PatrolStartLocationKey);
	FVector PatrolEndLocation = BlackboardComp->GetValueAsVector(UGRBTTask_FindRandomLocation::PatrolEndLocationKey);
	
	UNavigationPath* PathToPatrolStart = NavSys->FindPathToLocationSynchronously(AIPawn->GetWorld(), AILocation, PatrolStartLocation);
	float PathLengthToPatrolStart = 0.0f;
	if (PathToPatrolStart && PathToPatrolStart->IsValid())
	{
		PathLengthToPatrolStart = PathToPatrolStart->GetPathLength();
	}

	UNavigationPath* PathToPatrolEnd = NavSys->FindPathToLocationSynchronously(AIPawn->GetWorld(), AILocation, PatrolEndLocation);
	float PathLengthToPatrolEnd = 0.0f;
	if (PathToPatrolEnd && PathToPatrolEnd->IsValid())
	{
		PathLengthToPatrolEnd = PathToPatrolEnd->GetPathLength();
	}
	
	if (PathLengthToPatrolEnd < PathLengthToPatrolStart)
	{
		BlackboardComp->SetValueAsVector(AGRAIController::PatrolStartLocationKey,PatrolEndLocation);
		BlackboardComp->SetValueAsVector(UGRBTTask_FindRandomLocation::PatrolEndLocationKey,PatrolStartLocation);
	}

	return EBTNodeResult::Succeeded;
}
