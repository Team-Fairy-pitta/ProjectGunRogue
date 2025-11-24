// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/GRBTTask_UseAttackAbility.h"
#include "AIController.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

UGRBTTask_UseAttackAbility::UGRBTTask_UseAttackAbility()
{
	NodeName = TEXT("Use Attack Ability");

	bNotifyTick = true;
}

EBTNodeResult::Type UGRBTTask_UseAttackAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Pawn);
	if (!ASI) return EBTNodeResult::Failed;

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC) return EBTNodeResult::Failed;

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
	if (!AIController) return;
	
	APawn* Pawn = AIController->GetPawn();
	if (!Pawn) return;
	
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Pawn);
	if (!ASI) return;
	
	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC) return;
	
	FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Ability.Hit.Active"));
	if (!ASC->HasMatchingGameplayTag(Tag))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
