#include "AbilitySystem/Abilities/GRGameplayAbility_FireProjectile.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "Character/GRCharacter.h"
#include "Player/GRPlayerState.h"
#include "Weapon/GRWeaponDefinition.h"
#include "Weapon/GRProjectile.h"
#include "Kismet/KismetMathLibrary.h"

void UGRGameplayAbility_FireProjectile::FireWeapon()
{
	// 발사 위치/방향
	FVector CameraLocation, MuzzleLocation;
	FRotator CameraRotation, AdjustedRotation;
	GetFireStartAndDirection(CameraLocation, CameraRotation, MuzzleLocation, AdjustedRotation);

	// 조준점 찾기
	const FVector TargetPoint = FindTargetPointFromCamera(CameraLocation, AdjustedRotation);

	// Muzzle에서 타겟으로 향하는 방향 계산
	const FVector Direction = (TargetPoint - MuzzleLocation).GetSafeNormal();
	const FRotator SpawnRotation = Direction.Rotation();

	// 투사체 스폰
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor && AvatarActor->HasAuthority())
	{
		SpawnProjectile(MuzzleLocation, SpawnRotation);
	}

	// FX (투사체 자체가 시각적 요소)
	PlayFireFX(MuzzleLocation, TargetPoint);

	// 탄퍼짐/반동
	ApplySpreadAndRecoil();

	// 디버그
	DrawFireDebug(MuzzleLocation, TargetPoint, false, TargetPoint);

	UE_LOG(LogTemp, Log, TEXT("[FireProjectile] Fired - Direction: %s"),
		*Direction.ToString());
}

void UGRGameplayAbility_FireProjectile::SpawnProjectile(
	const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	ACharacter* Character;
	UAbilitySystemComponent* ASC;
	UGRCombatAttributeSet* CombatSet;
	if (!GetBasicComponents(Character, ASC, CombatSet))
	{
		return;
	}

	AGRCharacter* GRCharacter = Cast<AGRCharacter>(Character);
	if (!GRCharacter)
	{
		return;
	}

	AGRPlayerState* PS = GRCharacter->GetPlayerState<AGRPlayerState>();
	if (!PS)
	{
		return;
	}

	UGRWeaponDefinition* WeaponDef = PS->GetCurrentWeaponDefinition();
	if (!WeaponDef || !WeaponDef->ProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[FireProjectile] No ProjectileClass in WeaponDef"));
		return;
	}

	if (!WeaponDef->DamageEffect)
	{
		UE_LOG(LogTemp, Error, TEXT("[FireProjectile] No DamageEffect in WeaponDef"));
		return;
	}

	// 데미지 및 폭발 정보 가져오기
	bool bIsCritical = false;
	float FinalDamage = CalculateFinalDamage(CombatSet, nullptr, bIsCritical);
	float ExplosionRadius = CombatSet->GetExplosionRadius();
	float ExplosionFalloff = CombatSet->GetExplosionFalloff();

	UE_LOG(LogTemp, Log, TEXT("[FireProjectile] Spawning projectile - Damage: %.1f, Radius: %.1f, Falloff: %.2f"),
		FinalDamage, ExplosionRadius, ExplosionFalloff);

	// WeaponDefinition에서 투사체 설정 가져오기
	float ProjectileSpeed = WeaponDef->ProjectileSpeed;
	float GravityScale = WeaponDef->ProjectileGravityScale;
	float LifeSpan = WeaponDef->ProjectileLifeSpan;

	// 투사체 스폰
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GRCharacter;
	SpawnParams.Instigator = GRCharacter;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AGRProjectile* Projectile = GetWorld()->SpawnActor<AGRProjectile>(
		WeaponDef->ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	if (Projectile)
	{
		FVector Velocity = SpawnRotation.Vector() * WeaponDef->ProjectileSpeed;
		// WeaponDefinition에서 이펙트/사운드 가져오기
		Projectile->InitializeProjectile(
			GRCharacter,
			FinalDamage,
			ExplosionRadius,
			ExplosionFalloff,
			Velocity,
			WeaponDef->ProjectileGravityScale,
			WeaponDef->ProjectileLifeSpan,
			WeaponDef->DamageEffect,
			WeaponDef->ImpactEffectNiagara,   // <- WeaponDef에서
			WeaponDef->ImpactEffectCascade,   // <- WeaponDef에서
			WeaponDef->ImpactSound            // <- WeaponDef에서
		);

		UE_LOG(LogTemp, Log, TEXT("[FireProjectile] Projectile spawned successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[FireProjectile] Failed to spawn projectile"));
	}
}
