// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/GRBTTask_Attack.h"
#include "AI/GRAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

UGRBTTask_Attack::UGRBTTask_Attack()
	:AttackDelay(2.0f)
{
	NodeName=TEXT("Attack");

	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UGRBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FPauseMemory* MyMemory = reinterpret_cast<FPauseMemory*>(NodeMemory);
	MyMemory->bPaused = true;
	MyMemory->StartTime = OwnerComp.GetWorld()->GetTimeSeconds();

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon)
	{
		return EBTNodeResult::Failed;
	}

	AGRAIController* GRAICon = Cast<AGRAIController>(AICon);
	if (!GRAICon)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!IsValid(BlackboardComp))
	{
		return EBTNodeResult::Failed;
	}

	APawn* AIPawn = GRAICon->GetPawn();
	if (!IsValid(AIPawn))
	{
		return EBTNodeResult::Failed;
	}

	UObject* TargetObj = BlackboardComp->GetValueAsObject(AGRAIController::TargetPlayerKey);
	if (!IsValid(TargetObj))
	{
		return EBTNodeResult::Failed;
	}

	ACharacter* TargetChar = Cast<ACharacter>(TargetObj);
	if (!IsValid(TargetChar))
	{
		return EBTNodeResult::Failed;
	}

	StopMovement(GRAICon);
	
	return EBTNodeResult::InProgress;
}

void UGRBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon)
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

	float CurrentTime = OwnerComp.GetWorld()->GetTimeSeconds();
	FPauseMemory* MyMemory = reinterpret_cast<FPauseMemory*>(NodeMemory);
	if (CurrentTime - MyMemory->StartTime >= AttackDelay)
	{
		MyMemory->bPaused = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	UObject* TargetObj = BB->GetValueAsObject(AGRAIController::TargetPlayerKey);
	if (!IsValid(TargetObj))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	AActor* TargetPlayer = Cast<AActor>(TargetObj);
	if (!IsValid(TargetPlayer))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	AICon->SetFocus(TargetPlayer);
}

uint16 UGRBTTask_Attack::GetInstanceMemorySize() const
{
	return sizeof(FPauseMemory);
}

void UGRBTTask_Attack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!IsValid(AICon))
	{
		return;
	}

	ResumeMovement(AICon);
	
}

void UGRBTTask_Attack::StopMovement(AAIController* AIController)
{
	if (AIController)
	{
		APawn* AIPawn = AIController->GetPawn();
		if (AIPawn)
		{
			if (UCharacterMovementComponent* MoveComp = AIPawn->FindComponentByClass<UCharacterMovementComponent>())
			{
				//UE_LOG(LogTemp, Warning, TEXT("AI Attack : Stopping Movement"));
				
				MoveComp->StopMovementImmediately();
				MoveComp->DisableMovement();

				MoveComp->bUseControllerDesiredRotation = false; 
				AIPawn->bUseControllerRotationYaw = true;
			}
		}
	}
}

void UGRBTTask_Attack::ResumeMovement(AAIController* AIController)
{
	if (AIController)
	{
		APawn* AIPawn = AIController->GetPawn();
		if (AIPawn)
		{
			if (UCharacterMovementComponent* MoveComp = AIPawn->FindComponentByClass<UCharacterMovementComponent>())
			{
				//UE_LOG(LogTemp, Warning, TEXT("AI Attack : Resuming Movement"));

				AIController->ClearFocus(EAIFocusPriority::Gameplay);
				AIController->SetControlRotation(AIPawn->GetActorRotation());
				
				MoveComp->SetMovementMode(MOVE_Walking);
				MoveComp->Activate();
				MoveComp->bUseControllerDesiredRotation = true; 
				AIPawn->bUseControllerRotationYaw = false;
			}
		}
	}
}

