#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GRGameplayAbility_BladeWaveFire.generated.h"

class UGRAbilitySystemComponent;
class UGRSkillAttributeSet_MeleeSkill;
class UAnimMontage;
class AGRBladeWaveProjectile;
class UAbilityTask_PlayMontageAndWait;

UCLASS()
class GUNROGUE_API UGRGameplayAbility_BladeWaveFire : public UGRGameplayAbility
{
	GENERATED_BODY()

public:
	UGRGameplayAbility_BladeWaveFire();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "BladeWave|Anim")
	UAnimMontage* FireMontage = nullptr;

	double LastFireTimeSeconds = -1.0;

	UPROPERTY(EditDefaultsOnly, Category = "BladeWave|Tags")
	FGameplayTag Tag_BladeWaveMode;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

private:
	UGRAbilitySystemComponent* GetGRASC() const;
	const UGRSkillAttributeSet_MeleeSkill* GetSkillSet() const;

	bool CanFireNow(double NowSeconds, float FireInterval) const;

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = nullptr;

	UFUNCTION()
	void OnFireMontageCompleted();

	UFUNCTION()
	void OnFireMontageCancelled();
};
