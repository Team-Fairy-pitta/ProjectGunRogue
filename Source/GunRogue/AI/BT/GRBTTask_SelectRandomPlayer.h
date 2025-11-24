// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GRBTTask_SelectRandomPlayer.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRBTTask_SelectRandomPlayer : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UGRBTTask_SelectRandomPlayer();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
