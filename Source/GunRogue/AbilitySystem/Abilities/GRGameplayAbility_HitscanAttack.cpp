#include "AbilitySystem/Abilities/GRGameplayAbility_HitscanAttack.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemInterface.h"


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

	FireLineTrace();

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGRGameplayAbility_HitscanAttack::FireLineTrace()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] No Character!"));
		return;
	}

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

	// LineTrace 실행
	FVector TraceStart = CameraLocation;
	FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * FireRange);

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

	if (!DamageEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fire] No DamageEffect class"));
		return;
	}

	// 피해량 계산
	const UGRCombatAttributeSet* CombatSet = SourceASC->GetSet<UGRCombatAttributeSet>();
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
	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(Character);
	EffectContext.AddHitResult(HitResult);

	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(
		DamageEffect, 1.0f, EffectContext);

	if (!SpecHandle.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[Fire] Invalid SpecHandle"));
		return;
	}

	SpecHandle.Data->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag(FName("Attribute.Data.Damage")),
		CalculatedDamage);

	TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

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
