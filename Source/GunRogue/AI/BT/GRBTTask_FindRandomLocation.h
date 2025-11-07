// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GRBTTask_FindRandomLocation.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRBTTask_FindRandomLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UGRBTTask_FindRandomLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
private:
	static const FName PatrolRandomLocationKey;
	
	UPROPERTY(EditAnywhere, Category = "Patrol",meta=(AllowPrivateAccess="true"))
	float MinRadius;

	UPROPERTY(EditAnywhere, Category = "Patrol",meta=(AllowPrivateAccess="true"))
	float MaxRadius;
};
