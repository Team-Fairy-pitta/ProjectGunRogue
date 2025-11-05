// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GRBTService_UpdateDistance.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRBTService_UpdateDistance : public UBTService
{
	GENERATED_BODY()

public:
	UGRBTService_UpdateDistance();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
private:
	UPROPERTY(EditAnywhere, Category="AI",meta=(AllowPrivateAccess="true"))
	float AttackRange;

	static const FName IsInAttackRangeKey;
	static const FName LastPlayerLocationKey;
};
