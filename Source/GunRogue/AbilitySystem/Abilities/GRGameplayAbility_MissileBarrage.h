#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRGameplayAbility.h"
#include "GRGameplayAbility_MissileBarrage.generated.h"

class AGRProjectile;
class UNiagaraSystem;
class UGameplayEffect;
class UGRSkillAttributeSet_MissileBrg;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkillActivated);

UCLASS()
class GUNROGUE_API UGRGameplayAbility_MissileBarrage : public UGRGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGRGameplayAbility_MissileBarrage();
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

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

	UPROPERTY(BlueprintAssignable, Category = "GunRogue|Skill")
	FOnSkillActivated OnSkillActivated;

protected:
	// ======== 투사체 설정 ========

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Projectile")
	TSubclassOf<AGRProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Projectile")
	TSubclassOf<UGameplayEffect> DamageEffect;

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

	// 스폰 사운드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Missile Barrage|Spawn FX")
	TObjectPtr<USoundBase> SpawnSound;

private:
	void StartFiring();
	void FireNextMissile();
	TArray<FVector> CalculateSpawnLocations();

	UPROPERTY()
	TObjectPtr<UGRSkillAttributeSet_MissileBrg> SkillAttributeSet;

	FTimerHandle FireTimerHandle;
	TArray<FVector> SpawnLocations;
	int32 CurrentMissileIndex;
};
