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
