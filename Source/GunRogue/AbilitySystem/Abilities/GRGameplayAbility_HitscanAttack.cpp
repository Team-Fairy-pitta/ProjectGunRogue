#include "AbilitySystem/Abilities/GRGameplayAbility_HitscanAttack.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "Weapon/GRWeaponHandle.h"
#include "Weapon/GRWeaponInstance.h"
#include "Weapon/GRWeaponDefinition.h"
#include "Character/GRCharacter.h"
#include "Character/Attachment/GRAttachmentComponent.h"
#include "Player/GRPlayerState.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemInterface.h"
#include "TimerManager.h"

UGRGameplayAbility_HitscanAttack::UGRGameplayAbility_HitscanAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	bIsRecoilRecoveryActive = false;
}

void UGRGameplayAbility_HitscanAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 반동 회복 중지
	StopRecoilRecovery();

	// 연사 시작
	StartContinuousFire();
}

void UGRGameplayAbility_HitscanAttack::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// 연사 중지
	StopContinuousFire();

	// 사격 멈추면 반동 회복 시작
	StartRecoilRecovery();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGRGameplayAbility_HitscanAttack::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	// 버튼 떼면 어빌리티 종료
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGRGameplayAbility_HitscanAttack::StartContinuousFire()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return;
	}

	IAbilitySystemInterface* SourceASI = Cast<IAbilitySystemInterface>(Character);
	if (!SourceASI)
	{
		return;
	}

	UAbilitySystemComponent* SourceASC = SourceASI->GetAbilitySystemComponent();
	if (!SourceASC)
	{
		return;
	}

	const UGRCombatAttributeSet* CombatSet = SourceASC->GetSet<UGRCombatAttributeSet>();
	if (!CombatSet)
	{
		return;
	}

	// FireRate에 따른 연사 간격 계산
	const float FireRate = CombatSet->GetFireRate();

	// 즉시 첫 발 발사
	FireLineTrace();

	// FireRate가 0 이하면 단발 무기 (타이머 설정 안 함)
	if (FireRate <= 0.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("[Fire] Single-shot weapon (FireRate: %.2f)"), FireRate);
		return;
	}

	const float FireInterval = 1.0f / FireRate;

	// 타이머로 연사 시작 (WeakThis 사용)
	TWeakObjectPtr<UGRGameplayAbility_HitscanAttack> WeakThis(this);
	GetWorld()->GetTimerManager().SetTimer(
		FireTimerHandle,
		[WeakThis]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->FireLineTrace();
			}
		},
		FireInterval,
		true  // Loop
	);
}

void UGRGameplayAbility_HitscanAttack::StopContinuousFire()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	}
}

void UGRGameplayAbility_HitscanAttack::FireLineTrace()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] No Character!"));
		return;
	}

	IAbilitySystemInterface* SourceASI = Cast<IAbilitySystemInterface>(Character);
	if (!SourceASI)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] No Source ASI"));
		return;
	}

	UAbilitySystemComponent* SourceASC = SourceASI->GetAbilitySystemComponent();
	if (!SourceASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] No Source ASC"));
		return;
	}

	UGRCombatAttributeSet* CombatSet = const_cast<UGRCombatAttributeSet*>(SourceASC->GetSet<UGRCombatAttributeSet>());
	if (!CombatSet)
	{
		return;
	}

	const bool bIsServer = (SourceASC->GetOwnerRole() == ROLE_Authority);

	AGRCharacter* GRCharacter = Cast<AGRCharacter>(Character);
	AGRPlayerState* PS = GRCharacter ? GRCharacter->GetPlayerState<AGRPlayerState>() : nullptr;
	UGRWeaponDefinition* WeaponDef = PS ? PS->GetCurrentWeaponDefinition() : nullptr;
	FGRWeaponInstance* WeaponInstance = nullptr;

	// 카메라 위치/방향 가져오기
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

	// Muzzle 소켓 위치 가져오기
	FVector MuzzleLocation = CameraLocation; // 못 찾으면 카메라 위치 사용(폴백)
	bool bHasMuzzleSocket = false;

	if (GRCharacter)
	{
		// SkeletalMesh 무기 확인
		USkeletalMeshComponent* WeaponMesh = GRCharacter->GetEquippedWeaponMesh();
		if (WeaponMesh)
		{
			// 총구 소켓 이름들
			TArray<FName> PossibleSocketNames = {
				FName("Muzzle"),
				FName("muzzle")
			};

			for (const FName& SocketName : PossibleSocketNames)
			{
				if (WeaponMesh->DoesSocketExist(SocketName))
				{
					MuzzleLocation = WeaponMesh->GetSocketLocation(SocketName);
					bHasMuzzleSocket = true;
					UE_LOG(LogTemp, Verbose, TEXT("[Fire] Using socket '%s'"), *SocketName.ToString());
					break;
				}
			}
		}
	}

	if (!bHasMuzzleSocket)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] Muzzle socket not found - using camera location"));
	}

	// WeaponHandle에서 직접 WeaponInstance 가져오기
	if (bIsServer)
	{
		if (!PS)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Fire] No PlayerState! (Server)"));
			StopContinuousFire();
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			return;
		}

		FGRWeaponHandle* WeaponHandle = PS->GetActiveWeaponHandle();
		if (!WeaponHandle || !WeaponHandle->IsActive())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Fire] No active weapon! (Server)"));
			StopContinuousFire();
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			return;
		}

		WeaponInstance = WeaponHandle->GetWeaponInstanceRef();
		if (!WeaponInstance || !WeaponInstance->IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Fire] No valid weapon instance! (Server)"));
			StopContinuousFire();
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			return;
		}

		// 서버에서만 실제 탄약 체크/소모
		if (!WeaponInstance->CheckHasAmmo())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Fire] No ammo! (Server)"));
			if (GRCharacter)
			{
				GRCharacter->ServerRPC_PlayEmptyFireFX(MuzzleLocation);
			}
			StopContinuousFire();
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			return;
		}

		if (!WeaponInstance->ConsumeAmmo())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Fire] Failed to consume ammo (Server)"));
			StopContinuousFire();
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			return;
		}

		// UI 표시용 AttributeSet 업데이트 (서버 기준 값 복제됨)
		CombatSet->UpdateAmmoDisplay(WeaponInstance->GetCurrentAmmo(), WeaponInstance->GetMaxAmmo());
	}
	else
	{
		// CombatAttributeSet 기반으로만 탄약 여부를 가볍게 체크해서 조기 종료
		if (!CombatSet->CheckHasAmmo())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Fire] No ammo! (Client UI)"));
			StopContinuousFire();
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			return;
		}

		// 실제 탄약 수는 서버가 WeaponInstance에서 관리
		// 클라는 여기서 그냥 발사/시각 피드백만 수행
	}

	// 사격 애니메이션 재생
	if (WeaponDef && WeaponDef->FireAnimMontage)
	{
		Character->PlayAnimMontage(WeaponDef->FireAnimMontage, 1.0f);
		UE_LOG(LogTemp, Verbose, TEXT("[Fire] Playing FireAnimMontage: %s"),
			*WeaponDef->FireAnimMontage->GetName());
	}

	if (!DamageEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] No DamageEffect class"));
		return;
	}

	// 무기 스탯 가져오기
	const float Accuracy = CombatSet->GetAccuracy();
	const float Recoil = CombatSet->GetRecoil();
	const float CurrentSpread = CombatSet->GetCurrentSpread();

	// 탄퍼짐 적용 (Accuracy 낮을수록, CurrentSpread 높을수록 더 퍼짐)
	const float SpreadAngle = CurrentSpread * (1.0f - Accuracy);
	const float RandomPitch = FMath::RandRange(-SpreadAngle, SpreadAngle);
	const float RandomYaw = FMath::RandRange(-SpreadAngle, SpreadAngle);

	FRotator AdjustedRotation = CameraRotation;
	AdjustedRotation.Pitch += RandomPitch;
	AdjustedRotation.Yaw += RandomYaw;

	// 카메라에서 타겟 포인트 찾기
	FVector CameraTraceEnd = CameraLocation + (AdjustedRotation.Vector() * FireRange);
	FHitResult CameraHit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);
	QueryParams.bTraceComplex = false;
	QueryParams.bReturnPhysicalMaterial = false;

	GetWorld()->LineTraceSingleByChannel(
		CameraHit,
		CameraLocation,
		CameraTraceEnd,
		ECC_Visibility,
		QueryParams
	);

	FVector TargetPoint = CameraHit.bBlockingHit ? CameraHit.Location : CameraTraceEnd;

	// 총구에서 타겟 포인트로 발사(Line Trace 실행)
	FVector MuzzleToTarget = (TargetPoint - MuzzleLocation).GetSafeNormal();
	FVector TraceStart = MuzzleLocation;
	FVector TraceEnd = MuzzleLocation + (MuzzleToTarget * FireRange);

	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		QueryParams
	);

	// 총알 궤적 재생 (히트 여부 관계없이)
	FVector TracerEndPoint = bHit ? HitResult.Location : TraceEnd;
	if (bIsServer && GRCharacter)
	{
		// 발사 FX/사운드
		GRCharacter->ServerRPC_PlayFireFX(MuzzleLocation, TracerEndPoint);
	}


	// 탄퍼짐 수치 증가. non-const 캐스팅 필요 -> IncreaseSpread 함수가 non-const 멤버임
	UGRCombatAttributeSet* MutableCombatSet = const_cast<UGRCombatAttributeSet*>(CombatSet);
	MutableCombatSet->IncreaseSpread(SourceASC);
	
	// 반동 적용
	ApplyRecoil(Recoil);

#if WITH_EDITOR
	// 디버그 라인 그리기 (에디터 전용)
	const FColor DebugColor = bHit ? FColor::Red : FColor::Green;
	DrawDebugLine(GetWorld(), TraceStart, bHit ? HitResult.Location : TraceEnd,
		DebugColor, false, DebugLineDuration, 0, 2.0f);

	if (bHit)
	{
		DrawDebugSphere(GetWorld(), HitResult.Location, 20.0f, 12, FColor::Orange,
			false, DebugLineDuration);
	}
#endif

	if (!bHit)
	{
		UE_LOG(LogTemp, Log, TEXT("[Fire] Miss"));
		return;
	}

	if (bIsServer && GRCharacter)
	{
		GRCharacter->ServerRPC_PlayImpactFX(HitResult.Location);
	}

	AActor* HitActor = HitResult.GetActor();
	if (!HitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] Hit but no actor"));
		return;
	}

	// 피해 적용은 서버에서만
	if (SourceASC->GetOwnerRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Log, TEXT("[Fire] Client hit - not applying damage (will be handled by server)"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Fire] Hit: %s"), *HitActor->GetName());

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

	// 피해량 계산
	const UGRCombatAttributeSet* TargetCombatSet = TargetASC->GetSet<UGRCombatAttributeSet>();
	const float TargetReduction = TargetCombatSet ? TargetCombatSet->GetDamageReduction() : 0.0f;
	const bool bIsCritical = FMath::RandRange(0.0f, 1.0f) < 0.1f;  // TODO: 치명타 로직 부위 공격으로 대체(현재 임시 치명타 확률 10%)

	float CalculatedDamage = FallbackDamage;  // 기본값 (폴백)

	if (CombatSet)
	{
		CalculatedDamage = CombatSet->CalculateFinalWeaponDamage(bIsCritical, TargetReduction);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] No CombatAttributeSet - Using base damage: %.1f"), FallbackDamage);
	}

	// GameplayEffect 적용
	FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(Character);  // Source = Attacker
	EffectContext.AddHitResult(HitResult);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
		DamageEffect, 1.0f, EffectContext);

	if (!SpecHandle.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[Fire] Invalid SpecHandle"));
		return;
	}

	SpecHandle.Data->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag(FName("Attribute.Data.Damage")),
		CalculatedDamage);

	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

	UE_LOG(LogTemp, Log, TEXT("[Fire] Damage Applied: %.1f (Critical: %s, Target Reduction: %.2f)"),
		CalculatedDamage,
		bIsCritical ? TEXT("YES") : TEXT("NO"),
		TargetReduction);

#if WITH_EDITOR
	// 화면 출력 (에디터 전용)
	if (GEngine)
	{
		const FString Message = FString::Printf(
			TEXT("Damage: %.1f %s"),
			CalculatedDamage,
			bIsCritical ? TEXT("[CRITICAL!]") : TEXT("")
		);
		const FColor Color = bIsCritical ? FColor::Yellow : FColor::White;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, Color, Message);
	}
#endif
}

void UGRGameplayAbility_HitscanAttack::ApplyRecoil(float RecoilAmount)
{
	AGRCharacter* GRCharacter = Cast<AGRCharacter>(GetAvatarActorFromActorInfo());
	if (!GRCharacter)
	{
		return;
	}

	StopRecoilRecovery();

	// 내부 패턴 (고정값)
	const float BasePitchRecoil = 0.5f;     // 위쪽 기본 반동
	const float PitchVariation = 0.3f;       // 위쪽 랜덤 범위 (±)
	const float YawVariation = 0.2f;         // 좌우 랜덤 범위 (±)

	const float RecoilPitch = (BasePitchRecoil + FMath::RandRange(-PitchVariation, PitchVariation)) * RecoilAmount;
	const float RecoilYaw = FMath::RandRange(-YawVariation, YawVariation) * RecoilAmount;

	// 부드러운 카메라 시스템 사용
	GRCharacter->AddControllerPitchSmooth_Temporal(RecoilPitch);
	GRCharacter->AddControllerYawSmooth_Temporal(RecoilYaw);

	UE_LOG(LogTemp, Verbose, TEXT("[Recoil] Applied (%.1f) - Pitch: %.2f, Yaw: %.2f"),
		RecoilAmount, RecoilPitch, RecoilYaw);
}

void UGRGameplayAbility_HitscanAttack::StartRecoilRecovery()
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

	TWeakObjectPtr<UGRGameplayAbility_HitscanAttack> WeakThis(this);
	TWeakObjectPtr<AGRCharacter> WeakCharacter(GRCharacter);

	// 고정된 회복 딜레이 (0.15초)
	const float RecoveryDelay = 0.15f;

	GetWorld()->GetTimerManager().SetTimer(
		RecoilRecoveryTimerHandle,
		[WeakThis, WeakCharacter]()
		{
			if (WeakThis.IsValid() && WeakCharacter.IsValid())
			{
				WeakCharacter->ReturnToLastControllerRotation();
				WeakThis->bIsRecoilRecoveryActive = false;

				UE_LOG(LogTemp, Verbose, TEXT("[Recoil] Recovery completed"));
			}
		},
		RecoveryDelay,
		false
	);

	bIsRecoilRecoveryActive = true;
}

void UGRGameplayAbility_HitscanAttack::StopRecoilRecovery()
{
	if (GetWorld() && RecoilRecoveryTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(RecoilRecoveryTimerHandle);
		bIsRecoilRecoveryActive = false;
	}
}
