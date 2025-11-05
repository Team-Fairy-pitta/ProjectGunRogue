// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GRBTTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UGRBTTask_Attack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual uint16 GetInstanceMemorySize() const override;
	
private:
	UPROPERTY(EditAnywhere, Category="Task")
	float PauseDuration;
	
	void StopMovement(AAIController* AIController);
	
	void ResumeMovement(AAIController* AIController);
	
	struct FPauseMemory
	{
		float StartTime = 0.f;
		bool bPaused = false;
	};
};
