#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GRBaseConditionExecution.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRBaseConditionExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:
	/** 조건 판단 */
	virtual bool CheckCondition(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FAggregatorEvaluateParameters& EvalParams) const PURE_VIRTUAL(UGRBaseConditionExecution::CheckCondition, return false;);

	/** 조건 만족 시 결과 적용 */
	virtual void ApplyResult(const FGameplayEffectSpec & Spec,FGameplayEffectCustomExecutionOutput & OutExecutionOutput) const PURE_VIRTUAL(UGRBaseConditionExecution::ApplyResult);


};
