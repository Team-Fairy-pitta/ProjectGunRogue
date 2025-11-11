// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GRBTTask_SwapPatrolPointsIfClose.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRBTTask_SwapPatrolPointsIfClose : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UGRBTTask_SwapPatrolPointsIfClose();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
