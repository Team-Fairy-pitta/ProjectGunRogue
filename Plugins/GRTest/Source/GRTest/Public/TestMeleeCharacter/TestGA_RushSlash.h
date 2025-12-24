#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "TestGA_RushSlash.generated.h"

class UAbilityTask_ApplyRootMotionConstantForce;
class UGRGameplayEffect;

UCLASS()
class GRTEST_API UTestGA_RushSlash : public UGRGameplayAbility
{
	GENERATED_BODY()
	
public:
	UTestGA_RushSlash();

protected:

	//Dash
	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Dash")
	float DashDuration = 0.35f;

	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Dash")
	float DashStrength = 1800.0f;

	//Hit Check
	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Hit")
	float HitRadius = 60.0f;

	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Hit")
	float HitCheckInterval = 0.02f;

	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Hit")
	TEnumAsByte<ECollisionChannel> HitChannel = ECC_Pawn;

	//knockback
	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Knockback")
	float BaseKnockbackStrength = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Knockback")
	float BaseKnockbackUpward = 120.0f;

	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Knockback")
	float KnockbackMultiplierWhenUpgraded = 1.25f;

	//Augment GE
	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Augment")
	TSubclassOf<UGRGameplayEffect> AmmoRestoreEffect;

	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Augment")
	float AmmoRestoreAmount = 1.f;

	//Augment Tags
	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Tags")
	FGameplayTag Tag_AmmoRestoreOnHit;

	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Tags")
	FGameplayTag Tag_IncreaseKnockbackAndDamage;

	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|Tags")
	FGameplayTag Tag_PierceTargets;

	//SetByCaller Tag
	UPROPERTY(EditDefaultsOnly, Category = "RushSlash|SetByCaller")
	FGameplayTag DataTag_AmmoRestore;

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

private:
	void StartDash();
	void StartHitCheck();
	void StopHitCheck();
	void PerformHitCheck(const FGameplayAbilityActorInfo* ActorInfo);

	void ApplyKnockbackToTarget(
		AActor* TargetActor,
		const FVector& DashStartLocation,
		bool bIsPiercing,
		bool bIsKnockbackUpgraded) const;

	void ApplyAmmoRestoreIfNeeded(bool bAnyTargetHit);

	UFUNCTION()
	void OnDashFinished();
};
