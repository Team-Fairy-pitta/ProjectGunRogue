// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GRAIController.generated.h"

/**
 * 
 */
class UBehaviorTree;
class UBlackboardData;
class UBehaviorTreeComponent;
class UBlackboardComponent;

UCLASS()
class GUNROGUE_API AGRAIController : public AAIController
{
	GENERATED_BODY()

public:
	AGRAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	static const FName TargetPlayerKey;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;
	
	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UBlackboardData> BlackboardAsset;
	
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorComp;
	
	UPROPERTY()
	UBlackboardComponent* BlackboardComp;
};
