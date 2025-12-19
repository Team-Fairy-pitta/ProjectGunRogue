#include "Weapon/Option/ConditionsExecution/GRBaseConditionExecution.h"

void UGRBaseConditionExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	if (CheckCondition(ExecutionParams, EvalParams))
	{
		ApplyResult(Spec, OutExecutionOutput);
	}
}
