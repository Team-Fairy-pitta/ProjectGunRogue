// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GRBTTask_KeepDistance.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRBTTask_KeepDistance : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UGRBTTask_KeepDistance();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
private:
	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess=true))
	float MaintainDistance;

	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess=true))
	float SearchRadius;

	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess=true))
	float AcceptanceRadius ;
	
	bool bIsMoving;
	FAIRequestID MoveRequestID;
};
