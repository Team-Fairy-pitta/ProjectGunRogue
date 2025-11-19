// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/GRBTTask_KeepDistance.h"
#include "AI/GRAIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"

UGRBTTask_KeepDistance::UGRBTTask_KeepDistance()
	:MaintainDistance(500.0f)
	,SearchRadius(300.0f)
	,AcceptanceRadius(50.0f)
{
	NodeName=TEXT("Keep Distance");

	bNotifyTick = true;
	bCreateNodeInstance = false;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UGRBTTask_KeepDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!IsValid(BlackboardComp))
	{
		return EBTNodeResult::Failed;
	}

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!IsValid(AICon))
	{
		return EBTNodeResult::Failed;
	}

	APawn* AIPawn = AICon->GetPawn();
	if (!IsValid(AIPawn))
	{
		return EBTNodeResult::Failed;
	}

	FVector AILocation = AIPawn->GetActorLocation();

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(AIPawn->GetWorld());
	if (!IsValid(NavSys))
	{
		return EBTNodeResult::Failed;
	}

	UObject* TargetObj = BlackboardComp->GetValueAsObject(AGRAIController::TargetPlayerKey);
	if (!IsValid(TargetObj))
	{
		return EBTNodeResult::Failed;
	}
	
	ACharacter* TargetPlayer = Cast<ACharacter>(TargetObj);
	if (!IsValid(TargetPlayer))
	{
		return EBTNodeResult::Failed;
	}

	FVector TargetLocation = TargetPlayer->GetActorLocation();

	float CurrentDistance = FVector::Dist(AILocation, TargetLocation);

	if (MaintainDistance < CurrentDistance)
	{
		return EBTNodeResult::Failed;
	}

	FVector AwayDir = (AILocation - TargetLocation).GetSafeNormal();
	FVector DesiredLocation = AILocation + AwayDir * (MaintainDistance - CurrentDistance);
		
	FNavLocation ProjectedLoc;
	if (NavSys->ProjectPointToNavigation(DesiredLocation, ProjectedLoc, FVector(SearchRadius, SearchRadius, SearchRadius)))
	{
		DesiredLocation = ProjectedLoc.Location;
	}
	else
	{
		DesiredLocation = AILocation;
	}

	ACharacter* AICharacter = Cast<ACharacter>(AIPawn);
	if (!IsValid(AICharacter))
	{
		return EBTNodeResult::Failed;
	}

	UCharacterMovementComponent* MoveComp = AICharacter->GetCharacterMovement();
	if (!IsValid(MoveComp))
	{
		return EBTNodeResult::Failed;
	}
	
	MoveComp->bOrientRotationToMovement = false;
	MoveComp->bUseControllerDesiredRotation = false; 
	AIPawn->bUseControllerRotationYaw = true;
	
	EPathFollowingRequestResult::Type MoveResult = AICon->MoveToLocation(DesiredLocation, AcceptanceRadius, true, true, true, false, nullptr, true);
	if (MoveResult == EPathFollowingRequestResult::Failed)
	{
		return EBTNodeResult::Failed;
	}
	if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::InProgress;
}

void UGRBTTask_KeepDistance::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!IsValid(AICon))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	APawn* AIPawn = AICon->GetPawn();
	if (!IsValid(AIPawn))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!IsValid(BB))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	UObject* TargetObj = BB->GetValueAsObject(AGRAIController::TargetPlayerKey);
	if (!IsValid(TargetObj))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	ACharacter* TargetPlayer = Cast<ACharacter>(TargetObj);
	if (!IsValid(TargetPlayer))
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

	EPathFollowingStatus::Type Status = PFC->GetStatus();
	if (Status == EPathFollowingStatus::Idle)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	if (Status == EPathFollowingStatus::Waiting)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);
		return;
	}

	AICon->SetFocus(TargetPlayer);
}

void UGRBTTask_KeepDistance::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!IsValid(AICon))
	{
		return;
	}

	APawn* AIPawn = AICon->GetPawn();
	if (!IsValid(AIPawn))
	{
		return;
	}

	ACharacter* AICharacter = Cast<ACharacter>(AIPawn);
	if (!IsValid(AICharacter))
	{
		return;
	}

	UCharacterMovementComponent* MoveComp = AICharacter->GetCharacterMovement();
	if (!IsValid(MoveComp))
	{
		return;
	}

	MoveComp->bOrientRotationToMovement = false;
	MoveComp->bUseControllerDesiredRotation = true; 
	AIPawn->bUseControllerRotationYaw = false;

	AICon->ClearFocus(EAIFocusPriority::Gameplay);

}
