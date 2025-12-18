#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GRGameplayAbility_FireWeapon.generated.h"

class UGameplayEffect;
class AGRCharacter;
class AGRPlayerState;
class UGRCombatAttributeSet;
class UGRWeaponDefinition;
struct FGRWeaponInstance;

/**
 * 모든 무기 발사 타입의 베이스 클래스
 * - 히트스캔(라이플, 샷건)
 * - 투사체(로켓, 유탄) 등등
 */

UCLASS(Abstract)
class GUNROGUE_API UGRGameplayAbility_FireWeapon : public UGRGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGRGameplayAbility_FireWeapon();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:

	// 공통 설정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Fire")
	float FireRange = 20000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Fire")
	float FallbackDamage = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Debug")
	float DebugLineDuration = 2.0f;

	// 발사 파이프라인
	// 실제 발사 실행 (하위 클래스에서 구현)
	virtual void FireWeapon() PURE_VIRTUAL(UGRGameplayAbility_FireWeapon::FireWeapon, );
	void PerformFire();

	// 연사 제어
	void StartContinuousFire();
	void StopContinuousFire();

	FTimerHandle FireTimerHandle;

	// 기본 컴포넌트 검증
	// Character, ASC, CombatSet 가져오기 - 실패 시 로그 출력
	bool GetBasicComponents(
		ACharacter*& OutCharacter,
		UAbilitySystemComponent*& OutASC,
		UGRCombatAttributeSet*& OutCombatSet);

	// 탄약
	bool CheckAndConsumeAmmo();

	// 발사 위치/방향
	void GetFireStartAndDirection(
		FVector& OutCameraLocation,
		FRotator& OutCameraRotation,
		FVector& OutMuzzleLocation,
		FRotator& OutAdjustedRotation);

	FVector GetMuzzleLocation();

	// 조준점 계산 (히트스캔/투사체 공통)
	FVector FindTargetPointFromCamera(const FVector& CameraLocation, const FRotator& AdjustedRotation);

	// 데미지 계산
	float CalculateFinalDamage(
		const UGRCombatAttributeSet* SourceCombat,
		const UGRCombatAttributeSet* TargetCombat,
		bool& bOutIsCritical);

	// 데미지 Effect 적용 (히트스캔/투사체 공통)
	void ApplyDamageEffect(
		AActor* HitActor,
		const FHitResult& Hit,
		float Damage);

	// FX/사운드
	void PlayFireFX(const FVector& MuzzleLocation, const FVector& TraceEnd);
	void PlayEmptyFireFX(const FVector& MuzzleLocation);
	void PlayImpactFX(const FVector& ImpactLocation);

	// 탄퍼짐/반동
	void ApplySpreadAndRecoil();
	void ApplyRecoil(float RecoilAmount);

	void StartRecoilRecovery();
	void StopRecoilRecovery();

	FTimerHandle RecoilRecoveryTimerHandle;
	bool bIsRecoilRecoveryActive = false;

	// 디버그
	void DrawFireDebug(const FVector& Start, const FVector& End, bool bHit, const FVector& HitLocation);

	// 확률로 탄 소모x
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|NoConsume")
	FGameplayTag NoAmmoConsumeTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|NoConsume", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float NoAmmoConsumeChance;

	//DotDmamage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Dot")
	FGameplayTag DotOnHitTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Dot")
	FGameplayTag DotStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Dot")
	TSubclassOf<UGameplayEffect> DotDamageEffect;
};
