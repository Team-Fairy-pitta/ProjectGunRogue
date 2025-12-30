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

	//Augment GE
	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Augment")
	TSubclassOf<UGRGameplayEffect> AmmoRestoreEffect;

	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Augment")
	TSubclassOf<UGRGameplayEffect> KnockbackEffect;

	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Augment")
	TSubclassOf<UGRGameplayEffect> PierceEffect;

	//Augment Tags
	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Tags")
	FGameplayTag Tag_AmmoRestoreOnHit;

	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Tags")
	FGameplayTag Tag_IncreaseKnockbackAndDamage;

	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Tags")
	FGameplayTag Tag_PierceTargets;

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
		const FVector& DashStartLocation,
		bool bIsPiercing,
		bool bIsKnockbackUpgraded) const;

	void ApplyAmmoRestore();
	void ApplyDamageToTarget(AActor* TargetActor, const FHitResult& Hit, float Damage) const;

	UFUNCTION()
	void OnDashFinished();
	
};
