#include "AbilitySystem/Abilities/GRGameplayAbility_GiantBomb.h"
#include "Character/GRCharacter.h"
#include "Weapon/GRProjectile.h"
#include "AbilitySystem/Attributes/GRSkillAttributeSet_GiantBomb.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/OverlapResult.h" 
#include "DrawDebugHelpers.h"

UGRGameplayAbility_GiantBomb::UGRGameplayAbility_GiantBomb()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UGRGameplayAbility_GiantBomb::OnGiveAbility(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	// AttributeSet 한 번만 찾아서 저장
	if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

		SkillAttributeSet = const_cast<UGRSkillAttributeSet_GiantBomb*>(
			ASC->GetSet<UGRSkillAttributeSet_GiantBomb>());

		if (!SkillAttributeSet)
		{
			UE_LOG(LogTemp, Error, TEXT("[GiantBomb] Failed to find SkillAttributeSet"));
		}

		CombatAttributeSet = const_cast<UGRCombatAttributeSet*>(
			ASC->GetSet<UGRCombatAttributeSet>());
		if (!CombatAttributeSet)
		{
			UE_LOG(LogTemp, Warning, TEXT("[GiantBomb] Failed to find CombatAttributeSet"));
		}
	}
}

void UGRGameplayAbility_GiantBomb::ActivateAbility(
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
	PlaceBomb();

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGRGameplayAbility_GiantBomb::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
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

void UGRGameplayAbility_GiantBomb::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGRGameplayAbility_GiantBomb::PlaceBomb()
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

	if (!SkillAttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("[GiantBomb] No SkillAttributeSet"));
		return;
	}

	if (!ProjectileClass || !DamageEffect)
	{
		UE_LOG(LogTemp, Error, TEXT("[GiantBomb] Missing ProjectileClass or DamageEffect"));
		return;
	}

	const float FinalDamage = CalculateFinalSkillDamage();

	// 배치 위치 계산 (캐릭터 앞)
	const FVector CharacterLocation = Character->GetActorLocation();
	const FVector ForwardVector = Character->GetActorForwardVector();
	const FVector PlacementLocation = CharacterLocation + (ForwardVector * PlacementDistance);

	// 스폰 이펙트/사운드 재생
	TArray<FVector> SpawnLocations;
	SpawnLocations.Add(PlacementLocation);

	GRCharacter->Multicast_PlaySkillSpawnEffects(
		SpawnLocations,
		SpawnEffectNiagara,
		SpawnEffectCascade,
		SpawnEffectScale,
		SpawnSound,
		SkillAttributeSet->GetFuseTime()
	);

	// 폭탄 스폰
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Character;
	SpawnParams.Instigator = Character;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AGRProjectile* Bomb = GetWorld()->SpawnActor<AGRProjectile>(
		ProjectileClass,
		PlacementLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (Bomb)
	{
		Bomb->InitializeStaticBomb(
			GRCharacter,
			FinalDamage,
			SkillAttributeSet->GetExplosionRadius(),
			SkillAttributeSet->GetExplosionFalloff(),
			SkillAttributeSet->GetFuseTime(),
			DamageEffect,
			ExplosionEffectNiagara,
			ExplosionEffectCascade,
			ExplosionSound
		);

		float KnockbackRadius = 0.0f;
		if (USphereComponent* CollisionComp = Bomb->FindComponentByClass<USphereComponent>())
		{
			KnockbackRadius = CollisionComp->GetScaledSphereRadius();
		}

		// 주변 캐릭터 밀어내기
		KnockbackNearbyCharacters(
			PlacementLocation,
			KnockbackRadius,
			SkillAttributeSet->GetKnockbackForce()
		);

		OnSkillActivated.Broadcast(PlacementLocation);

		UE_LOG(LogTemp, Log, TEXT("[GiantBomb] Bomb placed at %s"), *PlacementLocation.ToString());
	}
}

void UGRGameplayAbility_GiantBomb::KnockbackNearbyCharacters(
	const FVector& BombLocation,
	float Radius,
	float Force)
{
	if (!GetWorld())
	{
		return;
	}

	// PVE 전용: AI 캐릭터는 서버에서만 움직이므로 클라이언트 복제 불필요
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetAvatarActorFromActorInfo());

	bool bHit = GetWorld()->OverlapMultiByChannel(
		Overlaps,
		BombLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(Radius),
		QueryParams
	);

	if (!bHit)
	{
		return;
	}

	for (const FOverlapResult& Overlap : Overlaps)
	{
		ACharacter* HitCharacter = Cast<ACharacter>(Overlap.GetActor());
		if (!HitCharacter)
		{
			continue;
		}

		UCharacterMovementComponent* MovementComp = HitCharacter->GetCharacterMovement();
		if (!MovementComp)
		{
			continue;
		}

		// 밀어내는 방향 계산
		FVector Direction = (HitCharacter->GetActorLocation() - BombLocation).GetSafeNormal2D();
		FVector LaunchVelocity = Direction * Force;
		LaunchVelocity.Z = Force * 0.3f; // 약간 위로

		// 밀어내기	적용
		MovementComp->AddImpulse(LaunchVelocity, true);

		UE_LOG(LogTemp, Log, TEXT("[GiantBomb] Knocked back: %s"), *HitCharacter->GetName());
	}
}

float UGRGameplayAbility_GiantBomb::CalculateFinalSkillDamage()
{
	if (!SkillAttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("[GiantBomb] No SkillAttributeSet for damage calculation"));
		return 0.0f;
	}

	// 스킬 기본 데미지
	const float SkillBaseDamage = SkillAttributeSet->GetBaseDamage();

	// CombatSet이 없으면 기본값만 반환
	if (!CombatAttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GiantBomb] No CombatSet - using base damage: %.1f"), SkillBaseDamage);
		return SkillBaseDamage;
	}

	return CombatAttributeSet->CalculateFinalSkillDamage(SkillBaseDamage, 0.0f);
}

float UGRGameplayAbility_GiantBomb::GetCooldown() const
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
