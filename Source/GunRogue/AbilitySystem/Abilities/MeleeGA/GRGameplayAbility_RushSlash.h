#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GRGameplayAbility_RushSlash.generated.h"

class UAbilityTask_ApplyRootMotionConstantForce;
class UGRGameplayEffect;
class UGRSkillAttributeSet_MeleeSkill;
class UAbilitySystemComponent;

UCLASS()
class GUNROGUE_API UGRGameplayAbility_RushSlash : public UGRGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGRGameplayAbility_RushSlash();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Hit")
	TEnumAsByte<ECollisionChannel> HitChannel = ECC_Pawn;

	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Damage")
	TSubclassOf<UGRGameplayEffect> DamageEffect;

	//Task
	UPROPERTY()
	UAbilityTask_ApplyRootMotionConstantForce* DashTask = nullptr;

	FVector PreviousActorLocation;
	FTimerHandle HitCheckTimerHandle;
	TSet<TWeakObjectPtr<AActor>> AlreadyHitActors;

protected:
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

	bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags,
		const FGameplayTagContainer* TargetTags,
		FGameplayTagContainer* OptionalRelevantTags) const;

private:

	const UGRSkillAttributeSet_MeleeSkill* GetSkillSet() const;

	void StartDash();
	void StopDash();
	void StartHitCheck();
	void StopHitCheck();
	void PerformHitCheck(const FGameplayAbilityActorInfo* ActorInfo);

	void ApplyKnockbackToTarget(
		AActor* TargetActor,
		const FVector& DashStartLocation);

	void ApplyDamageToTarget(AActor* TargetActor, const FHitResult& Hit, float Damage) const;

	UFUNCTION()
	void OnDashFinished();
	
};
