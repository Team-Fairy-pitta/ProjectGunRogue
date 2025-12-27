#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRGameplayAbility.h"
#include "GRGameplayAbility_GiantBomb.generated.h"

class AGRProjectile;
class UGRSkillAttributeSet_GiantBomb;
class UGRCombatAttributeSet;
class UGameplayEffect;
class UNiagaraSystem;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGiantBombActivated, FVector, Location);

UCLASS()
class GUNROGUE_API UGRGameplayAbility_GiantBomb : public UGRGameplayAbility
{
	GENERATED_BODY()

public:
	UGRGameplayAbility_GiantBomb();

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void ApplyCooldown(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	// UI용 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "GiantBomb|Events")
	FOnGiantBombActivated OnSkillActivated;

protected:
	// ======== 투사체 설정 ========

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GiantBomb|Projectile")
	TSubclassOf<AGRProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GiantBomb|Projectile")
	TSubclassOf<UGameplayEffect> DamageEffect;

	// ======== 배치 설정 ========

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GiantBomb|Placement")
	float PlacementDistance = 0.0f;

	// ======== FX / 사운드 설정 ========

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GiantBomb|ExplosionFX")
	TObjectPtr<UNiagaraSystem> ExplosionEffectNiagara;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GiantBomb|ExplosionFX")
	TObjectPtr<UParticleSystem> ExplosionEffectCascade;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GiantBomb|ExplosionFX")
	TObjectPtr<USoundBase> ExplosionSound;

	// ======== Spawn FX / 사운드 ========

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GiantBomb|SpawnFX")
	TObjectPtr<UNiagaraSystem> SpawnEffectNiagara;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GiantBomb|SpawnFX")
	TObjectPtr<UParticleSystem> SpawnEffectCascade;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GiantBomb|SpawnFX")
	float SpawnEffectScale = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GiantBomb|SpawnFX")
	TObjectPtr<USoundBase> SpawnSound;

private:
	void PlaceBomb();
	void KnockbackNearbyCharacters(const FVector& BombLocation, float Radius, float Force);

	float CalculateFinalSkillDamage();

	UPROPERTY()
	TObjectPtr<UGRCombatAttributeSet> CombatAttributeSet;

	UPROPERTY()
	TObjectPtr<UGRSkillAttributeSet_GiantBomb> SkillAttributeSet;

	float GetCooldown() const;
};
