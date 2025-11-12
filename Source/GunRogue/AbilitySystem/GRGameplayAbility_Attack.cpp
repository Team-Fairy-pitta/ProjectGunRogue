#include "GRGameplayAbility_Attack.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
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

		// 피해량 공식 적용
		// 타겟의 ASC 가져오기
		IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(HitActor);
		if (TargetASI)
		{
			UAbilitySystemComponent* TargetASC = TargetASI->GetAbilitySystemComponent();

			// 공격자의 ASC 가져오기
			IAbilitySystemInterface* SourceASI = Cast<IAbilitySystemInterface>(Character);
			UAbilitySystemComponent* SourceASC = SourceASI ? SourceASI->GetAbilitySystemComponent() : nullptr;

			if (TargetASC && SourceASC && DamageEffect)
			{
				// 1. 공격자의 CombatAttributeSet 가져오기
				const UGRCombatAttributeSet* CombatSet = SourceASC->GetSet<UGRCombatAttributeSet>();
				const UGRCombatAttributeSet* TargetCombatSet = TargetASC->GetSet<UGRCombatAttributeSet>();

				float TargetReduction = TargetCombatSet ? TargetCombatSet->GetDamageReduction() : 0.0f;

				// 2. 약점 판정 (임시: 10% 확률 - 향후 헤드샷 등으로 변경)
				bool bIsCritical = FMath::RandRange(0.0f, 1.0f) < 0.1f;

				// 3. 최종 피해량 계산
				float CalculatedDamage = Damage;  // 기본값 (폴백)

				if (CombatSet)
				{
					// 피해량 공식 적용
					CalculatedDamage = CombatSet->CalculateFinalDamage(bIsCritical, TargetReduction);
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

				if (SpecHandle.IsValid())
				{
					// 계산된 Damage 값 설정
					SpecHandle.Data->SetSetByCallerMagnitude(
						FGameplayTag::RequestGameplayTag(FName("Attribute.Data.Damage")),
						CalculatedDamage);

					TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

					// 로그 출력
					UE_LOG(LogTemp, Log, TEXT("[Fire] Damage Applied: %.1f (Critical: %s, Target Reduction: %.2f)"),
						CalculatedDamage,
						bIsCritical ? TEXT("YES") : TEXT("NO"),
						TargetReduction);

					// 화면 출력
					if (GEngine)
					{
						FString Message = FString::Printf(
							TEXT("Damage: %.1f %s"),
							CalculatedDamage,
							bIsCritical ? TEXT("[CRITICAL!]") : TEXT("")
						);
						FColor Color = bIsCritical ? FColor::Yellow : FColor::White;
						GEngine->AddOnScreenDebugMessage(-1, 2.0f, Color, Message);
					}
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
