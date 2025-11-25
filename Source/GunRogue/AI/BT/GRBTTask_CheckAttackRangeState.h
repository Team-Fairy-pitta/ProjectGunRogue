// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GRBTTask_CheckAttackRangeState.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRBTTask_CheckAttackRangeState : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UGRBTTask_CheckAttackRangeState();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess=true))
	float CloseRange;

	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess=true))
	float MidRange;
};
