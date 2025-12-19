#pragma once

#include "CoreMinimal.h"
#include "Weapon/Option/ConditionsExecution/GRBaseConditionExecution.h"
#include "GRAmmoPercentExecution.generated.h"

/**
 * 
 */


UCLASS()
class GUNROGUE_API UGRAmmoPercentExecution : public UGRBaseConditionExecution
{
	GENERATED_BODY()
	
public:
	UGRAmmoPercentExecution();

protected:
	virtual bool CheckCondition(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FAggregatorEvaluateParameters& EvalParams) const override;

	virtual void ApplyResult(const FGameplayEffectSpec& Spec, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
