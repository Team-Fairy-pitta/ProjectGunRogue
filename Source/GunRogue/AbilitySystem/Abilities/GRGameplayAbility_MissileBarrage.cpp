#include "AbilitySystem/Abilities/GRGameplayAbility_MissileBarrage.h"
#include "Character/GRCharacter.h"
#include "Weapon/GRProjectile.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

UGRGameplayAbility_MissileBarrage::UGRGameplayAbility_MissileBarrage()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UGRGameplayAbility_MissileBarrage::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// 쿨타임 체크
	if (bIsOnCooldown)
	{
		return false;
	}

	return true;
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

	// 델리게이트 브로드캐스트
	OnSkillActivated.Broadcast();

	// 스폰 위치 계산
	SpawnLocations = CalculateSpawnLocations();
	CurrentMissileIndex = 0;

	// 미사일 시각 효과 생성
	SpawnMissileVisuals();

	// 발사 시작
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

	UE_LOG(LogTemp, Log, TEXT("[MissileBarrage] Activated - %d missiles"), MissileCount);
}

void UGRGameplayAbility_MissileBarrage::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	}

	// 쿨타임 시작
	if (!bWasCancelled)
	{
		StartCooldown();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

float UGRGameplayAbility_MissileBarrage::CalculateSkillDamage()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return BaseDamage;
	}

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Character);
	if (!ASI)
	{
		return BaseDamage;
	}

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC)
	{
		return BaseDamage;
	}

	const UGRCombatAttributeSet* CombatSet = ASC->GetSet<UGRCombatAttributeSet>();
	if (!CombatSet)
	{
		return BaseDamage;
	}

	// 스킬 데미지 계산 (CombatSet의 스킬 공식 사용)
	const float SkillDamageFromAttributes = CombatSet->CalculateSkillDamage();

	// Ability의 BaseDamage와 합산
	const float TotalSkillDamage = BaseDamage + SkillDamageFromAttributes;

	// 최종 피해 배율 적용
	const float FinalMultiplier = CombatSet->CalculateFinalDamageMultiplier();
	const float FinalDamage = TotalSkillDamage * FinalMultiplier;

	UE_LOG(LogTemp, Log, TEXT("[MissileBarrage] Damage - Base: %.1f, AttributeBonus: %.1f, Final: %.1f"),
		BaseDamage, SkillDamageFromAttributes, FinalDamage);

	return FMath::Max(FinalDamage, 0.0f);
}

TArray<FVector> UGRGameplayAbility_MissileBarrage::CalculateSpawnLocations()
{
	TArray<FVector> Locations;

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return Locations;
	}

	const FVector CharacterLocation = Character->GetActorLocation();
	const FRotator CharacterRotation = Character->GetActorRotation();
	const float AngleStep = 360.0f / MissileCount;

	for (int32 i = 0; i < MissileCount; ++i)
	{
		const float Angle = FMath::DegreesToRadians(AngleStep * i);
		const FVector Offset(
			FMath::Cos(Angle) * SpawnRadius,
			FMath::Sin(Angle) * SpawnRadius,
			SpawnHeightOffset
		);

		const FVector RotatedOffset = CharacterRotation.RotateVector(Offset);
		Locations.Add(CharacterLocation + RotatedOffset);
	}

	return Locations;
}

void UGRGameplayAbility_MissileBarrage::SpawnMissileVisuals()
{
	AGRCharacter* GRCharacter = Cast<AGRCharacter>(GetAvatarActorFromActorInfo());
	if (GRCharacter && GRCharacter->HasAuthority())
	{
		// 서버에서 Multicast 호출
		GRCharacter->Multicast_PlaySkillSpawnEffects(
			SpawnLocations,
			SpawnEffectNiagara,
			SpawnEffectCascade,
			SpawnEffectScale
		);
	}
}

void UGRGameplayAbility_MissileBarrage::StartFiring()
{
	if (SpawnLocations.Num() == 0)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	FireNextMissile();

	if (MissileCount > 1 && FireInterval > 0.0f)
	{
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
}

void UGRGameplayAbility_MissileBarrage::FireNextMissile()
{
	if (CurrentMissileIndex >= SpawnLocations.Num())
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

	if (!ProjectileClass || !DamageEffect)
	{
		UE_LOG(LogTemp, Error, TEXT("[MissileBarrage] ProjectileClass or DamageEffect not set"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	const FVector SpawnLocation = SpawnLocations[CurrentMissileIndex];

	// 타겟 방향 계산
	FVector TargetDirection = GetTargetDirection();
	const float RandomPitch = FMath::RandRange(-SpreadAngle, SpreadAngle);
	const float RandomYaw = FMath::RandRange(-SpreadAngle, SpreadAngle);
	FRotator SpawnRotation = TargetDirection.Rotation();
	SpawnRotation.Pitch += RandomPitch;
	SpawnRotation.Yaw += RandomYaw;

	// 스킬 데미지 계산
	float FinalDamage = CalculateSkillDamage();

	// GRProjectile 스폰 (재사용!)
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Character;
	SpawnParams.Instigator = Character;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AGRProjectile* Projectile = GetWorld()->SpawnActor<AGRProjectile>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	if (Projectile)
	{
		const FVector Velocity = SpawnRotation.Vector() * ProjectileSpeed;

		// GRProjectile 초기화 (스킬 설정 전달)
		Projectile->InitializeProjectile(
			GRCharacter,
			FinalDamage,          // <- 계산된 스킬 데미지
			ExplosionRadius,      // <- Ability 설정
			ExplosionFalloff,     // <- Ability 설정
			Velocity,
			GravityScale,         // <- Ability 설정
			LifeSpan,             // <- Ability 설정
			DamageEffect,          // <- Ability의 DamageEffect
			ExplosionEffectNiagara,   // <- Ability 설정
			ExplosionEffectCascade,   // <- Ability 설정 (캐스케이드)
			ExplosionSound
		);

		UE_LOG(LogTemp, Log, TEXT("[MissileBarrage] Fired missile %d/%d - Damage: %.1f"),
			CurrentMissileIndex + 1, MissileCount, FinalDamage);
	}

	CurrentMissileIndex++;
}

FVector UGRGameplayAbility_MissileBarrage::GetTargetDirection()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return FVector::ForwardVector;
	}

	FVector CameraLocation;
	FRotator CameraRotation;

	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	if (PC)
	{
		PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
	}
	else
	{
		CameraLocation = Character->GetActorLocation();
		CameraRotation = Character->GetActorRotation();
	}

	const FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * TargetRange);
	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);

	GetWorld()->LineTraceSingleByChannel(
		Hit,
		CameraLocation,
		TraceEnd,
		ECC_Visibility,
		QueryParams
	);

	const FVector TargetPoint = Hit.bBlockingHit ? Hit.Location : TraceEnd;
	const FVector CharacterLocation = Character->GetActorLocation();

	return (TargetPoint - CharacterLocation).GetSafeNormal();
}

void UGRGameplayAbility_MissileBarrage::StartCooldown()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return;
	}

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Character);
	if (!ASI)
	{
		return;
	}

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	const UGRCombatAttributeSet* CombatSet = ASC->GetSet<UGRCombatAttributeSet>();

	// 쿨타임 감소 적용
	float CooldownReduction = 0.0f;
	if (CombatSet)
	{
		CooldownReduction = CombatSet->GetSkillCooldownReduction();
	}

	const float FinalCooldown = BaseCooldown * (1.0f - CooldownReduction);

	bIsOnCooldown = true;
	CooldownStartTime = GetWorld()->GetTimeSeconds();
	CooldownEndTime = CooldownStartTime + FinalCooldown;

	UE_LOG(LogTemp, Log, TEXT("[MissileBarrage] Cooldown started - Base: %.1f, Reduction: %.2f, Final: %.1f"),
		BaseCooldown, CooldownReduction, FinalCooldown);

	// UI 업데이트 타이머 (0.1초마다)
	TWeakObjectPtr<UGRGameplayAbility_MissileBarrage> WeakThis(this);
	GetWorld()->GetTimerManager().SetTimer(
		CooldownTimerHandle,
		[WeakThis]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->UpdateCooldownUI();
			}
		},
		0.1f,
		true
	);
}

void UGRGameplayAbility_MissileBarrage::UpdateCooldownUI()
{
	if (!GetWorld())
	{
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float RemainingCooldown = FMath::Max(0.0f, CooldownEndTime - CurrentTime);

	// 델리게이트 브로드캐스트
	OnCooldownChanged.Broadcast(RemainingCooldown, GetMaxCooldown());

	// 쿨타임 완료
	if (RemainingCooldown <= 0.0f)
	{
		bIsOnCooldown = false;
		GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);

		// 쿨타임 완료 델리게이트
		OnSkillReady.Broadcast();

		UE_LOG(LogTemp, Log, TEXT("[MissileBarrage] Cooldown finished!"));
	}
}

float UGRGameplayAbility_MissileBarrage::GetRemainingCooldown() const
{
	if (!bIsOnCooldown || !GetWorld())
	{
		return 0.0f;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	return FMath::Max(0.0f, CooldownEndTime - CurrentTime);
}

float UGRGameplayAbility_MissileBarrage::GetMaxCooldown() const
{
	return BaseCooldown;
}

bool UGRGameplayAbility_MissileBarrage::IsOnCooldown() const
{
	return bIsOnCooldown;
}
