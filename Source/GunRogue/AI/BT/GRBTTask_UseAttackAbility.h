// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GRBTTask_UseAttackAbility.generated.h"

class UGameplayAbility;

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRBTTask_UseAttackAbility : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UGRBTTask_UseAttackAbility();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, Category = "Ability",meta=(AllowPrivateAccess))
	TSubclassOf<UGameplayAbility> AbilityToUse;
};
