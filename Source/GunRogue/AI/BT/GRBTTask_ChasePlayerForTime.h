// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GRBTTask_ChasePlayerForTime.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRBTTask_ChasePlayerForTime : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UGRBTTask_ChasePlayerForTime();

protected:
	virtual uint16 GetInstanceMemorySize() const override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
private:
	struct FChaseMemory
	{
		float TimeElapsed;
	};
	
	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess=true))
	float ChaseDuration;
	
	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess=true))
	float AcceptanceRadius;
};
