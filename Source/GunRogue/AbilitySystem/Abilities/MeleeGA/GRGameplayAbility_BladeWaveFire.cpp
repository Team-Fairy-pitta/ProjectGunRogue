#include "AbilitySystem/Abilities/MeleeGA/GRGameplayAbility_BladeWaveFire.h"

#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRSkillAttributeSet_MeleeSkill.h"
#include "AbilitySystem/Abilities/MeleeGA/GRBladeWaveProjectile.h"

#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/UnrealType.h"
#include "Character/GRCharacter.h"

UGRGameplayAbility_BladeWaveFire::UGRGameplayAbility_BladeWaveFire()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	Tag_BladeWaveMode				= FGameplayTag::RequestGameplayTag(TEXT("State.BladeWaveMode"));
	Tag_SizeAndDamageUp				= FGameplayTag::RequestGameplayTag(TEXT("Augment.BladeWave.SizeAndDamageUp"));
	Tag_SlowPierceAndDamageUp		= FGameplayTag::RequestGameplayTag(TEXT("Augment.BladeWave.SlowPierceDamageUp"));
	Tag_KillReduceSupportCooldown	= FGameplayTag::RequestGameplayTag(TEXT("Augment.BladeWave.KillReduceCooldown"));

	ActivationRequiredTags.AddTag(Tag_BladeWaveMode);
}

UGRAbilitySystemComponent* UGRGameplayAbility_BladeWaveFire::GetGRASC() const
{
	return Cast<UGRAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
}

const UGRSkillAttributeSet_MeleeSkill* UGRGameplayAbility_BladeWaveFire::GetSkillSet() const
{
	const UGRAbilitySystemComponent* GRASC = GetGRASC();
	return GRASC ? GRASC->GetSet<UGRSkillAttributeSet_MeleeSkill>() : nullptr;
}

bool UGRGameplayAbility_BladeWaveFire::CanFireNow(double NowSeconds, float FireInterval) const
{
	return (LastFireTimeSeconds < 0.0) || ((NowSeconds - LastFireTimeSeconds) >= FireInterval);
}

void UGRGameplayAbility_BladeWaveFire::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UGRAbilitySystemComponent* GRASC = GetGRASC();

	const UGRSkillAttributeSet_MeleeSkill* SkillSet = GetSkillSet();

	ACharacter* OwnerChar = Cast<ACharacter>(GetAvatarActorFromActorInfo());

	if (!GRASC || !SkillSet || !OwnerChar || !ProjectileClass)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	float FireInterval = SkillSet->GetBladeWave_BaseFireInterval();

	const bool bSlowPierce = GRASC->HasMatchingGameplayTag(Tag_SlowPierceAndDamageUp);
	bool bPierce = false;
	if (bSlowPierce)
	{
		FireInterval *= SkillSet->GetBladeWave_SlowFireIntervalMultiplier();
		bPierce = true;
	}

	const double Now = GetWorld()->GetTimeSeconds();
	if (!CanFireNow(Now, FireInterval))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}
	LastFireTimeSeconds = Now;

	float Damage = SkillSet->GetBladeWave_BaseDamage();
	float WaveScale = SkillSet->GetBladeWave_BaseWaveScale();

	const bool bSizeUp = GRASC->HasMatchingGameplayTag(Tag_SizeAndDamageUp);
	if (bSizeUp)
	{
		WaveScale *= SkillSet->GetBladeWave_SizeAndDamageUp_ScaleMultiplier();
		Damage *= SkillSet->GetBladeWave_DamageMultiplier();
	}

	if (ActorInfo->IsNetAuthority())
	{
		const bool bSpawned = SpawnProjectileServer(Damage, WaveScale, bPierce);

		if (bSpawned && FireMontage && ActorInfo->IsLocallyControlled())
		{
			if (USkeletalMeshComponent* Mesh = OwnerChar->GetMesh())
			{
				if (UAnimInstance* Anim = Mesh->GetAnimInstance())
				{
					Anim->Montage_Play(FireMontage);
				}
			}
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

bool UGRGameplayAbility_BladeWaveFire::SpawnProjectileServer(float Damage, float WaveScale, bool bPierce) const
{
	UWorld* World = GetWorld();

	AGRCharacter* OwnerChar = Cast<AGRCharacter>(GetAvatarActorFromActorInfo());
	if (!World || !OwnerChar || !ProjectileClass)
	{
		return false;
	}

	USkeletalMeshComponent* WeaponMesh = OwnerChar->GetEquippedWeaponMesh();

	USkeletalMeshComponent* CharMesh = OwnerChar->GetMesh();

	FVector SpawnLocation = OwnerChar->GetActorLocation();

	if (WeaponMesh && WeaponMesh->DoesSocketExist(MuzzleSocketName))
	{
		SpawnLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);
	}

	const FRotator SpawnRotation = OwnerChar->GetBaseAimRotation();

	const FTransform SpawnTM(SpawnRotation, SpawnLocation);

	AGRBladeWaveProjectile* Proj = World->SpawnActorDeferred<AGRBladeWaveProjectile>(
		ProjectileClass,
		SpawnTM,
		OwnerChar,
		OwnerChar,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!Proj)
	{
		return false;
	}

	Proj->InitProjectile(Damage, WaveScale, bPierce);
	Proj->FinishSpawning(SpawnTM);

	return true;
}
