// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/GRBTTask_UseAttackAbility.h"
#include "AI/Controller/GRAIController.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AI/Controller/GRNormalAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

UGRBTTask_UseAttackAbility::UGRBTTask_UseAttackAbility()
	:AbilityToUse(nullptr)
	,bCanMove(true)
	,bCanRotate(false)
	,TargetPlayer(nullptr)
{
	NodeName = TEXT("Use Attack Ability");

	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UGRBTTask_UseAttackAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}
	
	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}
	
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(AIPawn);
	if (!ASI)
	{
		return EBTNodeResult::Failed;
	}
	
	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!IsValid(BlackboardComp))
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

	TargetPlayer=TargetChar;
	StopMovement(AIController);

	if (BlackboardComp->GetValueAsClass(AGRNormalAIController::FireWithWeaponAbilityKey))
	{
		AbilityToUse = BlackboardComp->GetValueAsClass(AGRNormalAIController::FireWithWeaponAbilityKey);
	}
	
	bool bActivated = ASC->TryActivateAbilityByClass(AbilityToUse);
	if (bActivated)
	{
		return EBTNodeResult::InProgress;
	}
	else
	{
		return EBTNodeResult::Failed;
	}
}

void UGRBTTask_UseAttackAbility::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(AIPawn);
	if (!ASI)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	FGameplayTag ActiveTag = FGameplayTag::RequestGameplayTag(FName("Enemy.Ability.Activating"));
	if (!ASC->HasMatchingGameplayTag(ActiveTag))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UGRBTTask_UseAttackAbility::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
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

void UGRBTTask_UseAttackAbility::StopMovement(AAIController* AIController)
{
	if (AIController)
	{
		APawn* AIPawn = AIController->GetPawn();
		if (AIPawn)
		{
			if (UCharacterMovementComponent* MoveComp = AIPawn->FindComponentByClass<UCharacterMovementComponent>())
			{
				if (!bCanMove)
				{
					MoveComp->StopMovementImmediately();
					MoveComp->DisableMovement();	
				}

				if (bCanRotate)
				{
					MoveComp->bUseControllerDesiredRotation = false; 
					AIPawn->bUseControllerRotationYaw = true;
					AIController->SetFocus(TargetPlayer);
				}
			}
		}
	}
}

void UGRBTTask_UseAttackAbility::ResumeMovement(AAIController* AIController)
{
	if (AIController)
	{
		APawn* AIPawn = AIController->GetPawn();
		if (AIPawn)
		{
			if (UCharacterMovementComponent* MoveComp = AIPawn->FindComponentByClass<UCharacterMovementComponent>())
			{
				if (!bCanMove)
				{
					MoveComp->SetMovementMode(MOVE_Walking);
					MoveComp->Activate();
				}

				if (bCanRotate)
				{
					AIController->ClearFocus(EAIFocusPriority::Gameplay);
					MoveComp->bUseControllerDesiredRotation = true; 
					AIPawn->bUseControllerRotationYaw = false;
					AIController->SetControlRotation(AIPawn->GetActorRotation());
				}
			}
		}
	}
}
