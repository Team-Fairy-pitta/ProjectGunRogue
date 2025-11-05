// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/GRBTService_UpdateDistance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/GRAIController.h"

const FName UGRBTService_UpdateDistance::IsInAttackRangeKey="IsInAttackRange";
const FName UGRBTService_UpdateDistance::LastPlayerLocationKey="LastPlayerLocation";

UGRBTService_UpdateDistance::UGRBTService_UpdateDistance()
	:AttackRange(300.0f)
{
	NodeName=TEXT("UpdateDistance");

	Interval=0.5f;
	RandomDeviation = 0.2f;
}

void UGRBTService_UpdateDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return;

	UObject* TargetPlayerObj = BlackboardComp->GetValueAsObject(AGRAIController::TargetPlayerKey);
	AActor* TargetPlayerActor = Cast<AActor>(TargetPlayerObj);
	if (!TargetPlayerActor)
	{
		BlackboardComp->SetValueAsBool(IsInAttackRangeKey, false);
		return;
	}

	BlackboardComp->SetValueAsVector(LastPlayerLocationKey, TargetPlayerActor->GetActorLocation());
	
	float DistanceSq = FVector::DistSquared(AIPawn->GetActorLocation(), TargetPlayerActor->GetActorLocation());
	bool bInRange = DistanceSq <= (AttackRange * AttackRange);

	BlackboardComp->SetValueAsBool(IsInAttackRangeKey, bInRange);
}
