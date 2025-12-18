#include "AbilitySystem/Abilities/GRGameplayAbility_FireWeapon.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "Weapon/GRWeaponHandle.h"
#include "Weapon/GRWeaponInstance.h"
#include "Weapon/GRWeaponDefinition.h"
#include "Character/GRCharacter.h"
#include "Player/GRPlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystemInterface.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"

UGRGameplayAbility_FireWeapon::UGRGameplayAbility_FireWeapon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	NoAmmoConsumeTag = FGameplayTag::RequestGameplayTag(FName("Item.Ammo.NoConsume"));
	NoAmmoConsumeChance = 0.2f;

	DotOnHitTag = FGameplayTag::RequestGameplayTag(FName("Item.Dot.OnHit"));
	DotStateTag = FGameplayTag::RequestGameplayTag(FName("Item.Dot.DotState"));
}

void UGRGameplayAbility_FireWeapon::ActivateAbility(
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

	StopRecoilRecovery();
	StartContinuousFire();
}

void UGRGameplayAbility_FireWeapon::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	StopContinuousFire();
	StartRecoilRecovery();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGRGameplayAbility_FireWeapon::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

// 기본 컴포넌트 검증
bool UGRGameplayAbility_FireWeapon::GetBasicComponents(
	ACharacter*& OutCharacter,
	UAbilitySystemComponent*& OutASC,
	UGRCombatAttributeSet*& OutCombatSet)
{
	OutCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!OutCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] No valid Character"));
		return false;
	}

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OutCharacter);
	if (!ASI)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] Character has no AbilitySystemInterface"));
		return false;
	}

	OutASC = ASI->GetAbilitySystemComponent();
	if (!OutASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] No AbilitySystemComponent"));
		return false;
	}

	OutCombatSet = const_cast<UGRCombatAttributeSet*>(
		OutASC->GetSet<UGRCombatAttributeSet>());
	if (!OutCombatSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] No CombatAttributeSet"));
		return false;
	}

	return true;
}

// 연사 제어
void UGRGameplayAbility_FireWeapon::StartContinuousFire()
{
	ACharacter* Character;
	UAbilitySystemComponent* ASC;
	UGRCombatAttributeSet* CombatSet;

	if (!GetBasicComponents(Character, ASC, CombatSet))
	{
		return;
	}

	const float FireRate = CombatSet->GetFireRate();

	// 즉시 첫 발 발사
	PerformFire();

	// FireRate가 0 이하면 단발 무기
	if (FireRate <= 0.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("[Fire] Single-shot weapon (FireRate: %.2f)"), FireRate);
		return;
	}

	// 연사 타이머
	const float FireInterval = 1.0f / FireRate;
	TWeakObjectPtr<UGRGameplayAbility_FireWeapon> WeakThis(this);

	GetWorld()->GetTimerManager().SetTimer(
		FireTimerHandle,
		[WeakThis]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->PerformFire();
			}
		},
		FireInterval,
		true
	);
}

void UGRGameplayAbility_FireWeapon::StopContinuousFire()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	}
}

void UGRGameplayAbility_FireWeapon::PerformFire()
{
	ACharacter* Character;
	UAbilitySystemComponent* ASC;
	UGRCombatAttributeSet* CombatSet;

	if (!GetBasicComponents(Character, ASC, CombatSet))
	{
		return;
	}

	// 탄약 체크/소모
	if (!CheckAndConsumeAmmo())
	{
		StopContinuousFire();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	// 애니메이션
	AGRCharacter* GRCharacter = Cast<AGRCharacter>(Character);
	if (GRCharacter)
	{
		AGRPlayerState* PlayerState = GRCharacter->GetPlayerState<AGRPlayerState>();
		if (PlayerState)
		{
			UGRWeaponDefinition* WeaponDef = PlayerState->GetCurrentWeaponDefinition();
			if (WeaponDef && WeaponDef->FireAnimMontage)
			{
				Character->PlayAnimMontage(WeaponDef->FireAnimMontage, 1.0f);
			}
		}
	}

	// 하위 클래스에서 실제 발사 (히트스캔, 투사체 등)
	FireWeapon();
}

// 탄약
bool UGRGameplayAbility_FireWeapon::CheckAndConsumeAmmo()
{
	ACharacter* Character;
	UAbilitySystemComponent* ASC;
	UGRCombatAttributeSet* CombatSet;

	if (!GetBasicComponents(Character, ASC, CombatSet))
	{
		return false;
	}

	const bool bIsServer = (ASC->GetOwnerRole() == ROLE_Authority);
	const FVector MuzzleLocation = GetMuzzleLocation();

	bool bShouldConsumeAmmo = true;

	if (ASC && NoAmmoConsumeTag.IsValid())
	{
		if (ASC->HasMatchingGameplayTag(NoAmmoConsumeTag))
		{
			const float Roll = FMath::FRand(); // 0.0 ~ 1.0
			if (Roll < NoAmmoConsumeChance)
			{
				bShouldConsumeAmmo = false;
				UE_LOG(LogTemp, Log, TEXT("[Fire] Ammo NOT consumed (Tag active, Roll=%.2f, Chance=%.2f)"), Roll, NoAmmoConsumeChance);
			}
		}
	}

	if (bIsServer)
	{
		// 서버: WeaponInstance 기반 탄약 처리
		AGRCharacter* GRCharacter = Cast<AGRCharacter>(Character);
		if (!GRCharacter)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Fire] Server - No GRCharacter"));
			return false;
		}

		AGRPlayerState* PlayerState = GRCharacter->GetPlayerState<AGRPlayerState>();
		if (!PlayerState)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Fire] Server - No PlayerState"));
			return false;
		}

		FGRWeaponHandle* WeaponHandle = PlayerState->GetActiveWeaponHandle();
		if (!WeaponHandle || !WeaponHandle->IsActive())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Fire] Server - No active weapon"));
			return false;
		}

		FGRWeaponInstance* WeaponInstance = WeaponHandle->GetWeaponInstanceRef();
		if (!WeaponInstance || !WeaponInstance->IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Fire] Server - Invalid weapon instance"));
			return false;
		}

		if (!WeaponInstance->CheckHasAmmo())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Fire] Server - No ammo"));
			PlayEmptyFireFX(MuzzleLocation);
			return false;
		}

		if (bShouldConsumeAmmo)
		{
			if (!WeaponInstance->ConsumeAmmo())
			{
				UE_LOG(LogTemp, Warning, TEXT("[Fire] Server - Failed to consume ammo"));
				return false;
			}

			// UI 업데이트 (탄이 줄어든 경우만)
			CombatSet->UpdateAmmoDisplay(WeaponInstance->GetCurrentAmmo(), WeaponInstance->GetMaxAmmo());
		}
		else
		{
			// 탄을 안 썼으니 UI 업데이트는 필요 없음
			UE_LOG(LogTemp, Verbose, TEXT("[Fire] Server - Ammo kept (no-consume effect)"));
		}
	}
	else
	{
		// 클라: AttributeSet 기반 체크 (기존 로직 그대로)
		if (!CombatSet->CheckHasAmmo())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Fire] Client - No ammo"));
			PlayEmptyFireFX(MuzzleLocation);
			return false;
		}
	}

	return true;
}

// 발사 위치/방향
void UGRGameplayAbility_FireWeapon::GetFireStartAndDirection(
	FVector& OutCameraLocation,
	FRotator& OutCameraRotation,
	FVector& OutMuzzleLocation,
	FRotator& OutAdjustedRotation)
{
	ACharacter* Character;
	UAbilitySystemComponent* ASC;
	UGRCombatAttributeSet* CombatSet;

	if (!GetBasicComponents(Character, ASC, CombatSet))
	{
		return;
	}

	// 카메라 위치/방향
	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	if (PC)
	{
		PC->GetPlayerViewPoint(OutCameraLocation, OutCameraRotation);
	}
	else
	{
		OutCameraLocation = Character->GetActorLocation();
		OutCameraRotation = Character->GetActorRotation();
	}

	// Muzzle 위치
	OutMuzzleLocation = GetMuzzleLocation();

	// 탄퍼짐 적용
	const float Accuracy = CombatSet->GetAccuracy();
	const float CurrentSpread = CombatSet->GetCurrentSpread();
	const float SpreadAngle = CurrentSpread * (1.0f - Accuracy);
	const float RandomPitch = FMath::RandRange(-SpreadAngle, SpreadAngle);
	const float RandomYaw = FMath::RandRange(-SpreadAngle, SpreadAngle);

	OutAdjustedRotation = OutCameraRotation;
	OutAdjustedRotation.Pitch += RandomPitch;
	OutAdjustedRotation.Yaw += RandomYaw;
}

FVector UGRGameplayAbility_FireWeapon::GetMuzzleLocation()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return FVector::ZeroVector;
	}

	AGRCharacter* GRCharacter = Cast<AGRCharacter>(Character);
	if (!GRCharacter)
	{
		return Character->GetActorLocation();
	}

	USkeletalMeshComponent* WeaponMesh = GRCharacter->GetEquippedWeaponMesh();
	if (!WeaponMesh)
	{
		return Character->GetActorLocation();
	}

	// Muzzle 소켓 찾기
	const TArray<FName> SocketNames = { FName("Muzzle"), FName("muzzle") };
	for (const FName& SocketName : SocketNames)
	{
		if (WeaponMesh->DoesSocketExist(SocketName))
		{
			return WeaponMesh->GetSocketLocation(SocketName);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[Fire] Muzzle socket not found"));
	return Character->GetActorLocation();
}

// 타겟 탐색 및 트레이스
FVector UGRGameplayAbility_FireWeapon::FindTargetPointFromCamera(
	const FVector& CameraLocation,
	const FRotator& AdjustedRotation)
{
	const FVector TraceEnd = CameraLocation + (AdjustedRotation.Vector() * FireRange);

	FHitResult CameraHit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetAvatarActorFromActorInfo());
	QueryParams.bTraceComplex = false;

	GetWorld()->LineTraceSingleByChannel(
		CameraHit,
		CameraLocation,
		TraceEnd,
		ECC_Visibility,
		QueryParams
	);

	return CameraHit.bBlockingHit ? CameraHit.Location : TraceEnd;
}

// 히트 및 데미지 처리
void UGRGameplayAbility_FireWeapon::ApplyDamageEffect(
	AActor* HitActor,
	const FHitResult& Hit,
	float Damage)
{
	if (!DamageEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] No DamageEffect class"));
		return;
	}

	ACharacter* Character;
	UAbilitySystemComponent* SourceASC;
	UGRCombatAttributeSet* CombatSet;

	if (!GetBasicComponents(Character, SourceASC, CombatSet))
	{
		return;
	}

	IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(HitActor);
	if (!TargetASI)
	{
		UE_LOG(LogTemp, Log, TEXT("[Fire] Hit actor has no ASC"));
		return;
	}

	UAbilitySystemComponent* TargetASC = TargetASI->GetAbilitySystemComponent();
	if (!TargetASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] No Target ASC"));
		return;
	}

	FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(Character);
	EffectContext.AddHitResult(Hit);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
		DamageEffect, 1.0f, EffectContext);

	if (!SpecHandle.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[Fire] Invalid SpecHandle"));
		return;
	}

	if (TargetASC && DotStateTag.IsValid() && TargetASC->HasMatchingGameplayTag(DotStateTag))
	{
		const float Bonus = CombatSet ? CombatSet->GetBonusDamageVsDoT() : 0.0f;
		Damage *= (1.0f + Bonus);
	}

	SpecHandle.Data->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag(FName("Attribute.Data.Damage")),
		Damage);

	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

	UE_LOG(LogTemp, Log, TEXT("[Fire] Damage: %.1f"), Damage);

	if (DotDamageEffect && SourceASC && TargetASC && DotOnHitTag.IsValid() && SourceASC->HasMatchingGameplayTag(DotOnHitTag))
	{
		FGameplayEffectContextHandle DotContext = SourceASC->MakeEffectContext();
		DotContext.AddSourceObject(Character);
		DotContext.AddHitResult(Hit);

		FGameplayEffectSpecHandle DotSpecHandle = SourceASC->MakeOutgoingSpec(DotDamageEffect, 1.0f, DotContext);

		if (DotSpecHandle.IsValid())
		{
			SourceASC->ApplyGameplayEffectSpecToTarget(*DotSpecHandle.Data.Get(), TargetASC);
			UE_LOG(LogTemp, Log, TEXT("[Fire] Applied DoT to target (Item.Dot.OnHit)"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Fire] Invalid DoT SpecHandle"));
		}
	}

#if WITH_EDITOR
	if (GEngine)
	{
		const FString Msg = FString::Printf(TEXT("Damage: %.1f"), Damage);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, Msg);
	}
#endif
}

float UGRGameplayAbility_FireWeapon::CalculateFinalDamage(
	const UGRCombatAttributeSet* SourceCombat,
	const UGRCombatAttributeSet* TargetCombat,
	bool& bOutIsCritical)
{
	// TODO: 부위별 치명타
	bOutIsCritical = FMath::RandRange(0.0f, 1.0f) < 0.1f;

	if (!SourceCombat)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] No CombatSet - fallback: %.1f"), FallbackDamage);
		return FallbackDamage;
	}

	const float TargetReduction = TargetCombat ? TargetCombat->GetDamageReduction() : 0.0f;
	return SourceCombat->CalculateFinalWeaponDamage(bOutIsCritical, TargetReduction);
}

// FX/사운드
void UGRGameplayAbility_FireWeapon::PlayFireFX(
	const FVector& MuzzleLocation,
	const FVector& TraceEnd)
{
	AGRCharacter* GRCharacter = Cast<AGRCharacter>(GetAvatarActorFromActorInfo());
	if (!GRCharacter)
	{
		return;
	}

	// 로컬에서 즉시 재생 (클라이언트 예측)
	GRCharacter->PlayFireFXLocal(MuzzleLocation, TraceEnd);

	// 서버만 다른 클라이언트들에게 브로드캐스트
	if (GRCharacter->HasAuthority())
	{
		GRCharacter->Multicast_PlayFireFX(MuzzleLocation, TraceEnd);
	}
}

void UGRGameplayAbility_FireWeapon::PlayEmptyFireFX(const FVector& MuzzleLocation)
{
	AGRCharacter* GRCharacter = Cast<AGRCharacter>(GetAvatarActorFromActorInfo());
	if (!GRCharacter)
	{
		return;
	}

	// 로컬에서 즉시 재생
	GRCharacter->PlayEmptyFireFXLocal(MuzzleLocation);

	// 서버만 다른 클라이언트들에게 브로드캐스트
	if (GRCharacter->HasAuthority())
	{
		GRCharacter->Multicast_PlayEmptyFireFX(MuzzleLocation);
	}
}

void UGRGameplayAbility_FireWeapon::PlayImpactFX(const FVector& ImpactLocation)
{
	AGRCharacter* GRCharacter = Cast<AGRCharacter>(GetAvatarActorFromActorInfo());
	if (!GRCharacter)
	{
		return;
	}

	// 로컬에서 즉시 재생
	GRCharacter->PlayImpactFXLocal(ImpactLocation);

	// 서버만 다른 클라이언트들에게 브로드캐스트
	if (GRCharacter->HasAuthority())
	{
		GRCharacter->Multicast_PlayImpactFX(ImpactLocation);
	}
}

// 탄퍼짐/반동
void UGRGameplayAbility_FireWeapon::ApplySpreadAndRecoil()
{
	ACharacter* Character;
	UAbilitySystemComponent* ASC;
	UGRCombatAttributeSet* CombatSet;

	if (!GetBasicComponents(Character, ASC, CombatSet))
	{
		return;
	}

	CombatSet->IncreaseSpread(ASC);
	ApplyRecoil(CombatSet->GetRecoil());
}


void UGRGameplayAbility_FireWeapon::ApplyRecoil(float RecoilAmount)
{
	AGRCharacter* GRCharacter = Cast<AGRCharacter>(GetAvatarActorFromActorInfo());
	if (!GRCharacter)
	{
		return;
	}

	StopRecoilRecovery();

	const float BasePitchRecoil = 0.5f;
	const float PitchVariation = 0.3f;
	const float YawVariation = 0.2f;

	const float RecoilPitch = (BasePitchRecoil + FMath::RandRange(-PitchVariation, PitchVariation)) * RecoilAmount;
	const float RecoilYaw = FMath::RandRange(-YawVariation, YawVariation) * RecoilAmount;

	GRCharacter->AddControllerPitchSmooth_Temporal(RecoilPitch);
	GRCharacter->AddControllerYawSmooth_Temporal(RecoilYaw);

	UE_LOG(LogTemp, Verbose, TEXT("[Recoil] Applied (%.1f) - Pitch: %.2f, Yaw: %.2f"),
		RecoilAmount, RecoilPitch, RecoilYaw);
}

void UGRGameplayAbility_FireWeapon::StartRecoilRecovery()
{
	if (bIsRecoilRecoveryActive)
	{
		return;
	}

	AGRCharacter* GRCharacter = Cast<AGRCharacter>(GetAvatarActorFromActorInfo());
	if (!GRCharacter || !GetWorld())
	{
		return;
	}

	TWeakObjectPtr<UGRGameplayAbility_FireWeapon> WeakThis(this);
	TWeakObjectPtr<AGRCharacter> WeakCharacter(GRCharacter);

	const float RecoveryDelay = 0.15f;
	GetWorld()->GetTimerManager().SetTimer(
		RecoilRecoveryTimerHandle,
		[WeakThis, WeakCharacter]()
		{
			if (WeakThis.IsValid() && WeakCharacter.IsValid())
			{
				WeakCharacter->ReturnToLastControllerRotation();
				WeakThis->bIsRecoilRecoveryActive = false;
			}
		},
		RecoveryDelay,
		false
	);

	bIsRecoilRecoveryActive = true;
}

void UGRGameplayAbility_FireWeapon::StopRecoilRecovery()
{
	if (GetWorld() && RecoilRecoveryTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(RecoilRecoveryTimerHandle);
		bIsRecoilRecoveryActive = false;
	}
}

// 디버그
void UGRGameplayAbility_FireWeapon::DrawFireDebug(
	const FVector& Start,
	const FVector& End,
	bool bHit,
	const FVector& HitLocation)
{
#if WITH_EDITOR
	const FColor Color = bHit ? FColor::Red : FColor::Green;
	const FVector DebugEnd = bHit ? HitLocation : End;

	DrawDebugLine(GetWorld(), Start, DebugEnd, Color, false, DebugLineDuration, 0, 2.0f);

	if (bHit)
	{
		DrawDebugSphere(GetWorld(), HitLocation, 20.0f, 12, FColor::Orange, false, DebugLineDuration);
	}
#endif
}
