#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GRGameplayAbility_BladeWaveFire.generated.h"

class UGRAbilitySystemComponent;
class UGRSkillAttributeSet_MeleeSkill;
class UAnimMontage;
class AGRBladeWaveProjectile;

UCLASS()
class GUNROGUE_API UGRGameplayAbility_BladeWaveFire : public UGRGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGRGameplayAbility_BladeWaveFire();

protected:
	//Normal Attack Change Tag
	UPROPERTY(EditDefaultsOnly, Category = "BladeWave|Tags")
	FGameplayTag Tag_BladeWaveMode;

	//Augment Tags
	UPROPERTY(EditDefaultsOnly, Category = "BladeMode|Tags")
	FGameplayTag Tag_SizeAndDamageUp;

	UPROPERTY(EditDefaultsOnly, Category = "BladeMode|Tags")
	FGameplayTag Tag_SlowPierceAndDamageUp;

	UPROPERTY(EditDefaultsOnly, Category = "BladeMode|Tags")
	FGameplayTag Tag_KillReduceSupportCooldown;

	// Projectile
	UPROPERTY(EditDefaultsOnly, Category = "BladeMode|Projectile")
	TSubclassOf<AGRBladeWaveProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "BladeMode|Projectile")
	FName MuzzleSocketName = TEXT("muzzle");

	UPROPERTY(EditDefaultsOnly, Category = "BladeWave|Anim")
	UAnimMontage* FireMontage = nullptr;

	double LastFireTimeSeconds = -1.0;

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

private:
	UGRAbilitySystemComponent* GetGRASC() const;
	const UGRSkillAttributeSet_MeleeSkill* GetSkillSet() const;

	bool CanFireNow(double NowSeconds, float FireInterval) const;
	bool SpawnProjectileServer(float Damage, float WaveScale, bool bPierce) const;
};
