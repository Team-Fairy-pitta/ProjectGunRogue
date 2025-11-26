#include "AbilitySystem/Abilities/GRGameplayAbility_HitscanAttack.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
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
}

void UGRGameplayAbility_HitscanAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

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
	if (FireRate <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] Invalid FireRate: %.2f"), FireRate);
		return;
	}

	const float FireInterval = 1.0f / FireRate;

	// 즉시 첫 발 발사
	FireLineTrace();

	// 타이머로 연사 시작
	GetWorld()->GetTimerManager().SetTimer(
		FireTimerHandle,
		[this]()
		{
			if (HasAuthority(&CurrentActivationInfo))
			{
				FireLineTrace();
			}
		},
		FireInterval,
		true  // Loop
	);

	// 탄퍼짐 회복 타이머 시작
	GetWorld()->GetTimerManager().SetTimer(
		SpreadRecoveryTimerHandle,
		[this]()
		{
			UpdateSpreadRecovery(0.1f);
		},
		0.1f,
		true
	);
}

void UGRGameplayAbility_HitscanAttack::StopContinuousFire()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(SpreadRecoveryTimerHandle);
	}
}

void UGRGameplayAbility_HitscanAttack::UpdateSpreadRecovery(float DeltaTime)
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

	// 탄퍼짐 회복
	const float RecoveryRate = CombatSet->GetSpreadRecoveryRate();
	CurrentSpread = FMath::Max(0.0f, CurrentSpread - RecoveryRate * DeltaTime);
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

	const UGRCombatAttributeSet* CombatSet = SourceASC->GetSet<UGRCombatAttributeSet>();
	if (!CombatSet)
	{
		return;
	}

	if (!DamageEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] No DamageEffect class"));
		return;
	}

	// 무기 스탯 가져오기
	const float Accuracy = CombatSet->GetAccuracy();
	const float Recoil = CombatSet->GetRecoil();
	const float MaxSpread = CombatSet->GetMaxSpread();

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

	// 탄퍼짐 적용 (Accuracy 낮을수록, CurrentSpread 높을수록 더 퍼짐)
	const float SpreadAngle = CurrentSpread * (1.0f - Accuracy);
	const float RandomPitch = FMath::RandRange(-SpreadAngle, SpreadAngle);
	const float RandomYaw = FMath::RandRange(-SpreadAngle, SpreadAngle);

	FRotator AdjustedRotation = CameraRotation;
	AdjustedRotation.Pitch += RandomPitch;
	AdjustedRotation.Yaw += RandomYaw;

	// LineTrace 실행
	FVector TraceStart = CameraLocation;
	FVector TraceEnd = CameraLocation + (AdjustedRotation.Vector() * FireRange);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);
	QueryParams.bTraceComplex = false;
	QueryParams.bReturnPhysicalMaterial = false;

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		QueryParams
	);

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

	// 탄퍼짐 증가 (최대치 제한)
	CurrentSpread = FMath::Min(CurrentSpread + SpreadIncreasePerShot, MaxSpread);

	if (PC)
	{
		ApplyRecoil(PC, Recoil);
	}

	if (!bHit)
	{
		UE_LOG(LogTemp, Log, TEXT("[Fire] Miss"));
		return;
	}

	AActor* HitActor = HitResult.GetActor();
	if (!HitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] Hit but no actor"));
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
	const bool bIsCritical = FMath::RandRange(0.0f, 1.0f) < 0.1f;  // 임시: 10% 확률

	float CalculatedDamage = Damage;  // 기본값 (폴백)

	if (CombatSet)
	{
		CalculatedDamage = CombatSet->CalculateFinalWeaponDamage(bIsCritical, TargetReduction);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] No CombatAttributeSet - Using base damage: %.1f"), Damage);
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

void UGRGameplayAbility_HitscanAttack::ApplyRecoil(APlayerController* PC, float RecoilAmount)
{
	if (!PC)
	{
		return;
	}

	// 반동 (위쪽으로 카메라 흔들림)
	const float RecoilPitch = FMath::RandRange(RecoilAmount * -0.5f, RecoilAmount * -1.0f);
	const float RecoilYaw = FMath::RandRange(-RecoilAmount * 0.3f, RecoilAmount * 0.3f);

	PC->AddPitchInput(RecoilPitch);
	PC->AddYawInput(RecoilYaw);
}
