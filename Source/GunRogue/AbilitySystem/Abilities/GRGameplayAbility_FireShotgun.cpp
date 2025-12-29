#include "AbilitySystem/Abilities/GRGameplayAbility_FireShotgun.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AI/Character/GRAICharacter.h"
#include "Character/GRCharacter.h"
#include "Player/GRPlayerState.h"
#include "Weapon/GRWeaponDefinition.h"
#include "DrawDebugHelpers.h"

void UGRGameplayAbility_FireShotgun::FireWeapon()
{
	// WeaponDefinition 가져오기
	AGRCharacter* GRCharacter = Cast<AGRCharacter>(GetAvatarActorFromActorInfo());
	if (!GRCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Shotgun] No GRCharacter"));
		return;
	}

	AGRPlayerState* PlayerState = GRCharacter->GetPlayerState<AGRPlayerState>();
	if (!PlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Shotgun] No PlayerState"));
		return;
	}

	UGRWeaponDefinition* WeaponDef = PlayerState->GetCurrentWeaponDefinition();
	if (!WeaponDef)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Shotgun] No WeaponDefinition"));
		return;
	}

	// 산탄총 설정 가져오기
	const int32 PelletCount = WeaponDef->PelletCount;
	const float PelletSpreadAngle = WeaponDef->PelletSpreadAngle;

	// 발사 위치/방향
	FVector CameraLocation, MuzzleLocation;
	FRotator CameraRotation, AdjustedRotation;
	GetFireStartAndDirection(CameraLocation, CameraRotation, MuzzleLocation, AdjustedRotation);

	// 기본 조준점 (탄퍼짐 적용된 상태)
	const FVector BaseTargetPoint = FindTargetPointFromCamera(CameraLocation, AdjustedRotation);

	// 각 펠렛 발사
	for (int32 i = 0; i < PelletCount; ++i)
	{
		FireSinglePellet(MuzzleLocation, BaseTargetPoint, CameraLocation, AdjustedRotation, PelletSpreadAngle);
	}

	// FX (첫 번째 펠렛 기준으로 표시)
	PlayFireFX(MuzzleLocation, BaseTargetPoint);

	// 탄퍼짐/반동 (한 번만 적용)
	ApplySpreadAndRecoil();

	UE_LOG(LogTemp, Log, TEXT("[Shotgun] Fired %d pellets with spread angle %.2f"), PelletCount, PelletSpreadAngle);
}

void UGRGameplayAbility_FireShotgun::FireSinglePellet(const FVector& MuzzleLocation, const FVector& BaseTargetPoint, const FVector& CameraLocation, const FRotator& BaseCameraRotation, float PelletSpreadAngle)
{
	// 펠렛별 추가 스프레드 적용
	const FVector BaseDirection = (BaseTargetPoint - CameraLocation).GetSafeNormal();
	const FVector PelletDirection = ApplyPelletSpread(BaseDirection, PelletSpreadAngle);
	const FVector PelletTargetPoint = CameraLocation + (PelletDirection * FireRange);

	// 라인 트레이스
	FHitResult Hit;
	const bool bHit = TraceFromMuzzle(MuzzleLocation, PelletTargetPoint, Hit);

	// 디버그
	DrawFireDebug(MuzzleLocation, PelletTargetPoint, bHit, Hit.Location);

	// 히트 처리
	if (!bHit)
	{
		return;
	}

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

	// AI만 타격
	if (!HitActor->IsA(AGRAICharacter::StaticClass()))
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

	// 데미지 계산 (WeaponDamage_Base 그대로 사용)
	bool bIsCritical = false;
	const float Damage = CalculateFinalDamage(SourceCombat, TargetCombat, bIsCritical);

	// 데미지 적용
	ApplyDamageEffect(HitActor, Hit, Damage);

	UE_LOG(LogTemp, Verbose, TEXT("[Shotgun] Pellet Hit - Damage: %.1f"), Damage);
}

FVector UGRGameplayAbility_FireShotgun::ApplyPelletSpread(const FVector& BaseDirection, float SpreadAngle)
{
	// 펠렛별 랜덤 스프레드
	const float RandomPitch = FMath::RandRange(-SpreadAngle, SpreadAngle);
	const float RandomYaw = FMath::RandRange(-SpreadAngle, SpreadAngle);

	// 방향 벡터를 로테이터로 변환 후 스프레드 적용
	FRotator BaseRotation = BaseDirection.Rotation();
	BaseRotation.Pitch += RandomPitch;
	BaseRotation.Yaw += RandomYaw;

	return BaseRotation.Vector();
}
