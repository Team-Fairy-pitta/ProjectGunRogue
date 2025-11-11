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
	,bIsMoving(false)
{
	NodeName=TEXT("Keep Distance");

	bNotifyTick = true;    
	bCreateNodeInstance = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UGRBTTask_KeepDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	
	AActor* TargetPlayer = Cast<AActor>(BlackboardComp->GetValueAsObject(AGRAIController::TargetPlayerKey));
	if (!TargetPlayer)
	{
		return EBTNodeResult::Failed;
	}

	FVector TargetLocation = TargetPlayer->GetActorLocation();

	float CurrentDistance = FVector::Dist(AILocation, TargetLocation);

	if (MaintainDistance < CurrentDistance)
	{
		return EBTNodeResult::Failed;
	}
	
	//FRotator LookAtRot = (TargetLocation - AILocation).Rotation();
	//AIPawn->SetActorRotation(LookAtRot);

	FVector AwayDir = (AILocation - TargetLocation).GetSafeNormal();
	FVector DesiredLocation = AILocation + AwayDir * (MaintainDistance - CurrentDistance);
	//FVector DesiredLocation = TargetLocation + AwayDir * MaintainDistance;
		
	if (NavSys)
	{
		FNavLocation ProjectedLoc;
		if (NavSys->ProjectPointToNavigation(DesiredLocation, ProjectedLoc, FVector(SearchRadius, SearchRadius, SearchRadius)))
		{
			DesiredLocation = ProjectedLoc.Location;
		}
		else
		{
			DesiredLocation = AILocation;;
		}
	}

	EPathFollowingRequestResult::Type MoveResult = AICon->MoveToLocation(DesiredLocation, AcceptanceRadius, true, true, true, false, nullptr, true);
	if (MoveResult == EPathFollowingRequestResult::Failed || MoveResult == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		return EBTNodeResult::Failed;
	}

	bIsMoving = true;

	ACharacter* AICharacter = Cast<ACharacter>(AIPawn);
	if (!AICharacter)
	{
		return EBTNodeResult::Failed;
	}

	UCharacterMovementComponent* MoveComp = AICharacter->GetCharacterMovement();
	if (!MoveComp)
	{
		return EBTNodeResult::Failed;
	}
	
	MoveComp->bOrientRotationToMovement = false;
	MoveComp->bUseControllerDesiredRotation = false; 
	AIPawn->bUseControllerRotationYaw = false;
	
	return EBTNodeResult::InProgress;
	
}

void UGRBTTask_KeepDistance::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	if (!bIsMoving)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	APawn* AIPawn = AICon->GetPawn();
	if (!AIPawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	AActor* TargetPlayer = Cast<AActor>( BB->GetValueAsObject( AGRAIController::TargetPlayerKey ) );
	if (!TargetPlayer)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FVector AILoc = AIPawn->GetActorLocation();
	FVector TargetLoc = TargetPlayer->GetActorLocation();
	FRotator LookAtRot = (TargetLoc - AILoc).Rotation();
	AICon->SetControlRotation(LookAtRot);
	//AIPawn->SetActorRotation(LookAtRot);
	
	EPathFollowingStatus::Type Status = AICon->GetMoveStatus();
	if (Status == EPathFollowingStatus::Idle || Status == EPathFollowingStatus::Waiting)
	{
		bIsMoving = false;
	}
}

void UGRBTTask_KeepDistance::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon)
	{
		return;
	}

	APawn* AIPawn = AICon->GetPawn();
	if (!AIPawn)
	{
		return;
	}

	ACharacter* AICharacter = Cast<ACharacter>(AIPawn);
	if (!AICharacter)
	{
		return;
	}

	UCharacterMovementComponent* MoveComp = AICharacter->GetCharacterMovement();
	if (!MoveComp)
	{
		return;
	}

	MoveComp->bOrientRotationToMovement = false;
	MoveComp->bUseControllerDesiredRotation = true; 
	AIPawn->bUseControllerRotationYaw = false;
}
