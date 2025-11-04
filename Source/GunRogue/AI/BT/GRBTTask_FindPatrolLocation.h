// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GRBTTask_FindPatrolLocation.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRBTTask_FindPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UGRBTTask_FindPatrolLocation();

private:
	UPROPERTY(EditAnywhere, Category = "Patrol",meta=(AllowPrivateAccess="true"))
	float MinRadius;

	UPROPERTY(EditAnywhere, Category = "Patrol",meta=(AllowPrivateAccess="true"))
	float MaxRadius;

	static const FName PatrolRandomLocationKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
