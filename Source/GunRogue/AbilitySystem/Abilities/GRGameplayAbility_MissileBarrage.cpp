#include "AbilitySystem/Abilities/GRGameplayAbility_MissileBarrage.h"
#include "AbilitySystem/Attributes/GRSkillAttributeSet_MissileBrg.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "Character/GRCharacter.h"
#include "Weapon/GRProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameplayEffect.h"
#include "DrawDebugHelpers.h"

UGRGameplayAbility_MissileBarrage::UGRGameplayAbility_MissileBarrage()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	CurrentMissileIndex = 0;
}

void UGRGameplayAbility_MissileBarrage::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	// 한 번만 가져와서 캐싱
	if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

		SkillAttributeSet = const_cast<UGRSkillAttributeSet_MissileBrg*>(
			ActorInfo->AbilitySystemComponent->GetSet<UGRSkillAttributeSet_MissileBrg>());

		if (!SkillAttributeSet)
		{
			UE_LOG(LogTemp, Warning, TEXT("[MissileBarrage] SkillAttributeSet not found!"));
		}

		CombatAttributeSet = const_cast<UGRCombatAttributeSet*>(
			ASC->GetSet<UGRCombatAttributeSet>());
		if (!CombatAttributeSet)
		{
			UE_LOG(LogTemp, Warning, TEXT("[MissileBarrage] Failed to find CombatAttributeSet"));
		}
	}
}

void UGRGameplayAbility_MissileBarrage::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!SkillAttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("[MissileBarrage] SkillAttributeSet not found!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ExecuteBarrage();
}

void UGRGameplayAbility_MissileBarrage::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (CooldownGameplayEffectClass)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGameplayEffectClass);
		if (SpecHandle.IsValid() && SpecHandle.Data.Get())
		{
			FGameplayTag CoolDownTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.Cooldown.Magnitude"));
			float FinalCooldown = GetCooldown();
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(CoolDownTag, FinalCooldown);
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
		}
	}
}

void UGRGameplayAbility_MissileBarrage::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	CurrentMissileIndex = 0;
	SpawnLocations.Empty();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGRGameplayAbility_MissileBarrage::ExecuteBarrage()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character || !Character->HasAuthority())
	{
		return;
	}

	AGRCharacter* GRCharacter = Cast<AGRCharacter>(Character);
	if (!GRCharacter)
	{
		return;
	}

	OnSkillActivated.Broadcast();

	// 스폰 위치 계산
	SpawnLocations = CalculateSpawnLocations();
	CurrentMissileIndex = 0;

	// 스폰 이펙트 재생
	GRCharacter->Multicast_PlaySkillSpawnEffects(
		SpawnLocations,
		SpawnEffectNiagara,
		SpawnEffectCascade,
		SpawnEffectScale,
		SpawnSound,
		SkillAttributeSet->GetSpawnDelay() * 8.0f
	);

	// SpawnDelay 후 발사 시작
	float SpawnDelay = SkillAttributeSet->GetSpawnDelay();
	TWeakObjectPtr<UGRGameplayAbility_MissileBarrage> WeakThis(this);
	GetWorld()->GetTimerManager().SetTimer(
		FireTimerHandle,
		[WeakThis]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->StartFiring();
			}
		},
		SpawnDelay,
		false
	);

	UE_LOG(LogTemp, Log, TEXT("[MissileBarrage] Activated - %d missiles"),
		FMath::RoundToInt(SkillAttributeSet->GetMissileCount()));
}

void UGRGameplayAbility_MissileBarrage::StartFiring()
{
	if (!SkillAttributeSet)
	{
		return;
	}

	FireNextMissile();

	float FireInterval = SkillAttributeSet->GetFireInterval();

	TWeakObjectPtr<UGRGameplayAbility_MissileBarrage> WeakThis(this);
	GetWorld()->GetTimerManager().SetTimer(
		FireTimerHandle,
		[WeakThis]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->FireNextMissile();
			}
		},
		FireInterval,
		true
	);
}

void UGRGameplayAbility_MissileBarrage::FireNextMissile()
{
	if (!SkillAttributeSet)
	{
		return;
	}

	int32 MissileCount = FMath::RoundToInt(SkillAttributeSet->GetMissileCount());

	if (CurrentMissileIndex >= MissileCount || CurrentMissileIndex >= SpawnLocations.Num())
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character || !Character->HasAuthority())
	{
		CurrentMissileIndex++;
		return;
	}

	AGRCharacter* GRCharacter = Cast<AGRCharacter>(Character);
	if (!GRCharacter)
	{
		CurrentMissileIndex++;
		return;
	}

	// 데미지 계산
	const float FinalDamage = CalculateFinalSkillDamage();

	// 카메라 위치 및 회전 가져오기
	FVector CameraLocation;
	FRotator CameraRotation;
	GRCharacter->GetActorEyesViewPoint(CameraLocation, CameraRotation);

	// AttributeSet에서 값 가져옴
	float TargetRange = SkillAttributeSet->GetTargetRange();
	float SpreadAngle = SkillAttributeSet->GetSpreadAngle();

	FVector ForwardDir = CameraRotation.Vector();
	float RandomYaw = FMath::FRandRange(-SpreadAngle, SpreadAngle);
	float RandomPitch = FMath::FRandRange(-SpreadAngle, SpreadAngle);
	FRotator SpreadRotation = ForwardDir.Rotation() + FRotator(RandomPitch, RandomYaw, 0.0f);
	FVector TargetPoint = CameraLocation + SpreadRotation.Vector() * TargetRange;

	const FVector SpawnLocation = SpawnLocations[CurrentMissileIndex];
	const FVector Direction = (TargetPoint - SpawnLocation).GetSafeNormal();
	const FRotator SpawnRotation = Direction.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GRCharacter;
	SpawnParams.Instigator = GRCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AGRProjectile* Projectile = GetWorld()->SpawnActor<AGRProjectile>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	if (Projectile)
	{
		float ProjectileSpeed = SkillAttributeSet->GetProjectileSpeed();
		FVector Velocity = Direction * ProjectileSpeed;

		Projectile->InitializeProjectile(
			GRCharacter,
			FinalDamage,
			SkillAttributeSet->GetExplosionRadius(),
			SkillAttributeSet->GetExplosionFalloff(),
			Velocity,
			SkillAttributeSet->GetGravityScale(),
			SkillAttributeSet->GetLifeSpan(),
			DamageEffect,
			ExplosionEffectNiagara,
			ExplosionEffectCascade,
			ExplosionSound
		);
	}

	CurrentMissileIndex++;
}

TArray<FVector> UGRGameplayAbility_MissileBarrage::CalculateSpawnLocations()
{
	TArray<FVector> Locations;

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return Locations;
	}

	if (!SkillAttributeSet)
	{
		return Locations;
	}

	int32 MissileCount = FMath::RoundToInt(SkillAttributeSet->GetMissileCount());
	float SpawnRadius = SkillAttributeSet->GetSpawnRadius();
	float SpawnHeightOffset = SkillAttributeSet->GetSpawnHeightOffset();

	const FVector CenterLocation = Character->GetActorLocation() + FVector(0, 0, SpawnHeightOffset);

	for (int32 i = 0; i < MissileCount; ++i)
	{
		float Angle = (360.0f / MissileCount) * i;
		FVector Offset = FVector(
			FMath::Cos(FMath::DegreesToRadians(Angle)) * SpawnRadius,
			FMath::Sin(FMath::DegreesToRadians(Angle)) * SpawnRadius,
			0.0f
		);
		Locations.Add(CenterLocation + Offset);
	}

	return Locations;
}

float UGRGameplayAbility_MissileBarrage::CalculateFinalSkillDamage()
{
	if (!SkillAttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("[MissileBarrage] No SkillAttributeSet for damage calculation"));
		return 0.0f;
	}

	const float SkillBaseDamage = SkillAttributeSet->GetBaseDamage();

	if (!CombatAttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MissileBarrage] No CombatSet - using base damage: %.1f"), SkillBaseDamage);
		return SkillBaseDamage;
	}

	return CombatAttributeSet->CalculateFinalSkillDamage(SkillBaseDamage, 0.0f);
}

float UGRGameplayAbility_MissileBarrage::GetCooldown() const
{
	if (!SkillAttributeSet)
	{
		return 0.0f;
	}

	float BaseCooldown = SkillAttributeSet->GetBaseCooldown();
	float CooldownReduction = SkillAttributeSet->GetCooldownReduction();
	float CombetSetCooldownReduction = CombatAttributeSet->GetSkillCooldownReduction();
	float FinalCooldown = BaseCooldown * (1.0f - CooldownReduction) * (1.0f - CombetSetCooldownReduction);

	return FinalCooldown;
}
