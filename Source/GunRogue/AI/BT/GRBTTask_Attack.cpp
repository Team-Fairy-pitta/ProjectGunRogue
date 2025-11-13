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

	//NOTE : 실행이 제대로 안됨 추후 수정 또는 다른 방법 찾아보기
	//TODO : 공격을 시작할 때 플레이어를 바라보도록 회전하게 하기
	// AIPawn->bUseControllerRotationYaw = true;
	// FVector AILocation = AIPawn->GetActorLocation();
	// FVector TargetLocation = TargetChar->GetActorLocation();
	// FRotator LookAtRot = (TargetLocation - AILocation).Rotation();
	// GRAICon->SetControlRotation(LookAtRot);
	
	return EBTNodeResult::InProgress;
}

void UGRBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	FPauseMemory* MyMemory = reinterpret_cast<FPauseMemory*>(NodeMemory);
	if (!MyMemory->bPaused)
	{
		return;
	}

	float CurrentTime = OwnerComp.GetWorld()->GetTimeSeconds();
	if (CurrentTime - MyMemory->StartTime >= AttackDelay)
	{
		AAIController* AICon = OwnerComp.GetAIOwner();
		if (AICon)
		{
			ResumeMovement(AICon);
		}

		MyMemory->bPaused = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

uint16 UGRBTTask_Attack::GetInstanceMemorySize() const
{
	return Super::GetInstanceMemorySize();
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
				UE_LOG(LogTemp, Warning, TEXT("AI Attack : Stopping Movement"));

				// NOTE : 둘 중 어느 것을 써야할지 아직 못 정함.
				// TODO : 동작을 멈출 경우 회전도 같이 못하게 막혀있는 것인지, 확인해보기
				//MoveComp->StopMovementImmediately();
				//MoveComp->DisableMovement();
				MoveComp->SetMovementMode(MOVE_None);
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
				UE_LOG(LogTemp, Warning, TEXT("AI Attack : Resuming Movement"));
				
				MoveComp->SetMovementMode(MOVE_Walking);
				MoveComp->Activate();
			}
		}
	}
}

