#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRGameplayAbility.h"
#include "GRGameplayAbility_Reload.generated.h"

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class GUNROGUE_API UGRGameplayAbility_Reload : public UGRGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGRGameplayAbility_Reload();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags,
		const FGameplayTagContainer* TargetTags,
		FGameplayTagContainer* OptionalRelevantTags) const override;

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

private:
	void PerformReload();
	
	// 애니메이션 몽타주 완료 콜백
	UFUNCTION()
	void OnReloadMontageCompleted();

	UFUNCTION()
	void OnReloadMontageCancelled();

	// PlayMontageAndWait 태스크 참조
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> PlayMontageTask;
};
