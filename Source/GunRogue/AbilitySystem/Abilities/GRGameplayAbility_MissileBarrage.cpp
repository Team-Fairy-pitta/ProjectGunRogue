#include "AbilitySystem/Abilities/GRGameplayAbility_MissileBarrage.h"
#include "AbilitySystem/Attributes/GRSkillAttributeSet_MissileBrg.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "Character/GRCharacter.h"
#include "Weapon/GRProjectile.h"
#include "Kismet/KismetMathLibrary.h"
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
		SkillAttributeSet = const_cast<UGRSkillAttributeSet_MissileBrg*>(
			ActorInfo->AbilitySystemComponent->GetSet<UGRSkillAttributeSet_MissileBrg>());

		if (!SkillAttributeSet)
		{
			UE_LOG(LogTemp, Warning, TEXT("[MissileBarrage] SkillAttributeSet not found!"));
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

	OnSkillActivated.Broadcast();

	SpawnLocations = CalculateSpawnLocations();
	CurrentMissileIndex = 0;

	AGRCharacter* GRCharacter = Cast<AGRCharacter>(GetAvatarActorFromActorInfo());
	if (GRCharacter && GRCharacter->HasAuthority())
	{
		GRCharacter->Multicast_PlaySkillSpawnEffects(
			SpawnLocations,
			SpawnEffectNiagara,
			SpawnEffectCascade,
			SpawnEffectScale,
			SpawnSound
		);
	}

	// SpawnDelay를 AttributeSet에서 가져옴
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

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->HasAuthority())
	{
		CurrentMissileIndex++;
		return;
	}

	AGRCharacter* GRCharacter = Cast<AGRCharacter>(AvatarActor);
	if (!GRCharacter)
	{
		CurrentMissileIndex++;
		return;
	}

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
			SkillAttributeSet->GetBaseDamage(),
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

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
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

	const FVector CenterLocation = AvatarActor->GetActorLocation() + FVector(0, 0, SpawnHeightOffset);

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
