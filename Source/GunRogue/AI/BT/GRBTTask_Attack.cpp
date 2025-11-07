// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/GRBTTask_Attack.h"
#include "AI/GRAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

UGRBTTask_Attack::UGRBTTask_Attack()
	:AttackDuration(2.0f)
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
	
	StopMovement(GRAICon);
	
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
	if (CurrentTime - MyMemory->StartTime >= AttackDuration)
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
				UE_LOG(LogTemp,Warning,TEXT("AI Attack : Stopping Movement"));
				
				MoveComp->StopMovementImmediately();
				MoveComp->DisableMovement();
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
				UE_LOG(LogTemp,Warning,TEXT("AI Attack : Resuming Movement"));
				
				MoveComp->SetMovementMode(MOVE_Walking);
				MoveComp->Activate();
			}
		}
	}
}
