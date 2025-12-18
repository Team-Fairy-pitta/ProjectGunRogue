#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRGameplayAbility.h"
#include "GRGameplayAbility_MissileBarrage.generated.h"

class AGRProjectile;
class UNiagaraSystem;
class UGameplayEffect;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillCooldownChanged, float, CurrentCooldown, float, MaxCooldown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkillActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkillReady);

UCLASS()
class GUNROGUE_API UGRGameplayAbility_MissileBarrage : public UGRGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGRGameplayAbility_MissileBarrage();

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

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags,
		const FGameplayTagContainer* TargetTags,
		FGameplayTagContainer* OptionalRelevantTags) const override;

	// UI용 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "GunRogue|Skill")
	FOnSkillCooldownChanged OnCooldownChanged;

	UPROPERTY(BlueprintAssignable, Category = "GunRogue|Skill")
	FOnSkillActivated OnSkillActivated;

	UPROPERTY(BlueprintAssignable, Category = "GunRogue|Skill")
	FOnSkillReady OnSkillReady;

	// 쿨타임 정보 조회 (UI용)
	UFUNCTION(BlueprintCallable, Category = "GunRogue|Skill")
	float GetRemainingCooldown() const;

	UFUNCTION(BlueprintCallable, Category = "GunRogue|Skill")
	float GetMaxCooldown() const;

	UFUNCTION(BlueprintCallable, Category = "GunRogue|Skill")
	bool IsOnCooldown() const;

protected:
	// ======== 투사체 설정 ========

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Projectile")
	TSubclassOf<AGRProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Projectile")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Projectile")
	float BaseDamage = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Projectile")
	float ExplosionRadius = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Projectile")
	float ExplosionFalloff = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Projectile")
	float ProjectileSpeed = 3000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Projectile")
	float GravityScale = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Projectile")
	float LifeSpan = 10.0f;

	// ======== 스폰 패턴 ========

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Spawn", meta = (ClampMin = "1", ClampMax = "50"))
	int32 MissileCount = 12;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Spawn")
	float SpawnRadius = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Spawn")
	float SpawnHeightOffset = 100.0f;

	// ======== 타이밍 ========

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Timing")
	float FireInterval = 0.08f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Timing")
	float SpawnDelay = 0.3f;

	// ======== 쿨타임 ========

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Cooldown")
	float BaseCooldown = 10.0f;

	// ======== 조준 ========

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Targeting")
	float TargetRange = 5000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Targeting")
	float SpreadAngle = 10.0f;

	// ======== 폭발 이펙트/사운드 ========

	// 나이아가라 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Explosion FX")
	TObjectPtr<UNiagaraSystem> ExplosionEffectNiagara;

	// 캐스케이드 이펙트 (나이아가라 없을 시 대체)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Explosion FX")
	TObjectPtr<UParticleSystem> ExplosionEffectCascade;

	// 폭발 사운드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Explosion FX")
	TObjectPtr<USoundBase> ExplosionSound;

	// ======== 스폰 VFX ========

	// 나이아가라
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Spawn FX")
	TObjectPtr<UNiagaraSystem> SpawnEffectNiagara;

	// 캐스케이드 (나이아가라 없을 시 대체)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Spawn FX")
	TObjectPtr<UParticleSystem> SpawnEffectCascade;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|VFX")
	float SpawnEffectScale = 1.0f;

private:
	float CalculateSkillDamage();
	TArray<FVector> CalculateSpawnLocations();
	void SpawnMissileVisuals();
	void StartFiring();
	void FireNextMissile();
	FVector GetTargetDirection();

	void StartCooldown();
	void UpdateCooldownUI();

	FTimerHandle FireTimerHandle;
	FTimerHandle CooldownTimerHandle;
	TArray<FVector> SpawnLocations;
	int32 CurrentMissileIndex = 0;

	float CooldownStartTime = 0.0f;
	float CooldownEndTime = 0.0f;
	bool bIsOnCooldown = false;
};
