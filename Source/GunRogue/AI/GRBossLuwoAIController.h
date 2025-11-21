// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GRBossLuwoAIController.generated.h"

/**
 * 
 */

class UBehaviorTree;
class UBlackboardData;
class UBehaviorTreeComponent;
class UBlackboardComponent;

UENUM(BlueprintType)
enum class EBossAttackRangeState : uint8
{
	None UMETA(DisplayName = "None"),
	Close UMETA(DisplayName = "Close"),
	Middle UMETA(DisplayName = "Middle"),
	Far UMETA(DisplayName = "Far"),
};

UCLASS()
class GUNROGUE_API AGRBossLuwoAIController : public AAIController
{
	GENERATED_BODY()

public:
	AGRBossLuwoAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	static const FName TargetPlayerKey;
	static const FName LastPlayerLocationKey;
	static const FName BossAttackRangeStateKey;
	static const FName FarAttackRandomIndexKey;

private:
	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;
	
	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UBlackboardData> BlackboardAsset;
	
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorComp;
	
	UPROPERTY()
	UBlackboardComponent* BlackboardComp;
};
