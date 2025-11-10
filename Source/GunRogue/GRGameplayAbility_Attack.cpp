#include "GRGameplayAbility_Attack.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemInterface.h"


UGRGameplayAbility_Attack::UGRGameplayAbility_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGRGameplayAbility_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FireLineTrace();

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGRGameplayAbility_Attack::FireLineTrace()
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

	// LineTrace 시작/끝 지점 계산
	FVector TraceStart = CameraLocation;
	FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * FireRange);

	// LineTrace 실행
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

	// 디버그 라인 그리기
	FColor DebugColor = bHit ? FColor::Red : FColor::Green;
	DrawDebugLine(GetWorld(), TraceStart, bHit ? HitResult.Location : TraceEnd,
		DebugColor, false, DebugLineDuration, 0, 2.0f);

	if (bHit)
	{
		DrawDebugSphere(GetWorld(), HitResult.Location, 20.0f, 12, FColor::Orange,
			false, DebugLineDuration);

		AActor* HitActor = HitResult.GetActor();
		UE_LOG(LogTemp, Warning, TEXT("[Fire] Hit: %s"), *HitActor->GetName());

		// AbilitySystemComponent 가져오기
		IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(HitActor);
		if (ASI)
		{
			UAbilitySystemComponent* TargetASC = ASI->GetAbilitySystemComponent();
			if (TargetASC && DamageEffect)
			{
				// GameplayEffect 적용
				FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
				EffectContext.AddSourceObject(Character);
				EffectContext.AddHitResult(HitResult);

				FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(
					DamageEffect, 1.0f, EffectContext);

				if (SpecHandle.IsValid())
				{
					// Damage 값 설정
					SpecHandle.Data->SetSetByCallerMagnitude(
						FGameplayTag::RequestGameplayTag(FName("Attribute.Data.Damage")), Damage);

					TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

					UE_LOG(LogTemp, Log, TEXT("[Fire] Damage Applied: %.1f"), Damage);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[Fire] No ASC or DamageEffect!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[Fire] Hit actor has no ASC"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[Fire] Miss"));
	}
}
