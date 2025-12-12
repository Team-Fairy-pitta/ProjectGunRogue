#include "AbilitySystem/Abilities/GRGameplayAbility_HitscanAttack.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

void UGRGameplayAbility_HitscanAttack::FireWeapon()
{
	// 발사 위치/방향
	FVector CameraLocation, MuzzleLocation;
	FRotator CameraRotation, AdjustedRotation;
	GetFireStartAndDirection(CameraLocation, CameraRotation, MuzzleLocation, AdjustedRotation);

	// 1. 조준점 찾기
	const FVector TargetPoint = FindTargetPointFromCamera(CameraLocation, AdjustedRotation);

	// 2. 라인 트레이스
	FHitResult Hit;
	const bool bHit = TraceFromMuzzle(MuzzleLocation, TargetPoint, Hit);

	// FX
	const FVector TraceEnd = bHit ? Hit.Location : TargetPoint;
	PlayFireFX(MuzzleLocation, TraceEnd);

	// 탄퍼짐/반동
	ApplySpreadAndRecoil();

	// 디버그
	DrawFireDebug(MuzzleLocation, TargetPoint, bHit, Hit.Location);

	// 히트 처리
	if (bHit)
	{
		HandleHit(Hit);
	}
}

bool UGRGameplayAbility_HitscanAttack::TraceFromMuzzle(
	const FVector& MuzzleLocation,
	const FVector& TargetPoint,
	FHitResult& OutHit)
{
	const FVector Direction = (TargetPoint - MuzzleLocation).GetSafeNormal();
	const FVector TraceEnd = MuzzleLocation + (Direction * FireRange);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetAvatarActorFromActorInfo());
	QueryParams.bTraceComplex = false;

	return GetWorld()->LineTraceSingleByChannel(
		OutHit,
		MuzzleLocation,
		TraceEnd,
		ECC_Visibility,
		QueryParams
	);
}

void UGRGameplayAbility_HitscanAttack::HandleHit(const FHitResult& Hit)
{
	PlayImpactFX(Hit.Location);

	AActor* HitActor = Hit.GetActor();
	if (!HitActor)
	{
		return;
	}

	// 서버만 데미지 처리
	ACharacter* Character;
	UAbilitySystemComponent* SourceASC;
	UGRCombatAttributeSet* SourceCombat;

	if (!GetBasicComponents(Character, SourceASC, SourceCombat))
	{
		return;
	}

	if (SourceASC->GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	// 타겟 ASC 가져오기
	IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(HitActor);
	if (!TargetASI)
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = TargetASI->GetAbilitySystemComponent();
	const UGRCombatAttributeSet* TargetCombat = TargetASC ? TargetASC->GetSet<UGRCombatAttributeSet>() : nullptr;

	// 데미지 계산
	bool bIsCritical = false;
	const float Damage = CalculateFinalDamage(SourceCombat, TargetCombat, bIsCritical);

	// 데미지 적용
	ApplyDamageEffect(HitActor, Hit, Damage);
}
